/*
 * Copyright (C) 2009, Willow Garage, Inc.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *   * Redistributions of source code must retain the above copyright notice,
 *     this list of conditions and the following disclaimer.
 *   * Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in the
 *     documentation and/or other materials provided with the distribution.
 *   * Neither the names of Stanford University or Willow Garage, Inc. nor the names of its
 *     contributors may be used to endorse or promote products derived from
 *     this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#include "resource_retriever/retriever.h"

#include <string.h>

#include <ros/package.h>
#include <ros/console.h>

#include <curl/curl.h>

#define MINIMAL_PROGRESS_FUNCTIONALITY_INTERVAL     3

namespace resource_retriever
{

class CURLStaticInit
{
public:
    CURLStaticInit()
        : initialized_(false)
    {
        CURLcode ret = curl_global_init(CURL_GLOBAL_ALL);
        if (ret != 0)
        {
            ROS_ERROR("Error initializing libcurl! retcode = %d", ret);
        }
        else
        {
            initialized_ = true;
        }
    }

    ~CURLStaticInit()
    {
        if (initialized_)
        {
            curl_global_cleanup();
        }
    }

    bool initialized_;
};
static CURLStaticInit g_curl_init;

Retriever::Retriever()
{
    curl_handle_ = curl_easy_init();
}

Retriever::~Retriever()
{
    if (curl_handle_)
    {
        curl_easy_cleanup(curl_handle_);
    }
}

struct MemoryBuffer
{
    std::vector<uint8_t> v;
};

size_t curlWriteFunc(void *buffer, size_t size, size_t nmemb, void *userp)
{
    MemoryBuffer *membuf = (MemoryBuffer *)userp;

    size_t prev_size = membuf->v.size();
    membuf->v.resize(prev_size + size * nmemb);
    memcpy(&membuf->v[prev_size], buffer, size * nmemb);

    return size * nmemb;
}

struct Progress Retriever::prog_;

//! TODO ADD MUTEX after trying !!!!
void Retriever::setProgress(struct Progress &prog)
{
    prog_ = prog;
}

struct Progress Retriever::getProgress()
{
    return prog_;
}

/* this is how the CURLOPT_XFERINFOFUNCTION callback works */
int Retriever::xferinfo(const void *p,
                        const curl_off_t dltotal, const curl_off_t dlnow,
                        const curl_off_t ultotal, const curl_off_t ulnow)
{
    struct Progress prog = *(struct Progress *)p;
    CURL *curl = prog.curl;
    double curtime_in_secs = 0;
    curl_easy_getinfo(curl, CURLINFO_TOTAL_TIME, &curtime_in_secs);

    /* under certain circumstances it may be desirable for certain functionality
       to only run every N seconds, in order to do this the transaction time can
       be used */
    if ((curtime_in_secs - prog.last_in_secs) >= MINIMAL_PROGRESS_FUNCTIONALITY_INTERVAL)
    {
        prog.last_in_secs = curtime_in_secs;
        prog.dl_total = dltotal;
        prog.dl_now = dlnow;
        setProgress(prog);
        //ROS_WARN("TOTAL TIME: %f \r\n", curtime_in_secs);
    }

    /*ROS_WARN("UP: %" CURL_FORMAT_CURL_OFF_T " of %" CURL_FORMAT_CURL_OFF_T
             "  DOWN: %" CURL_FORMAT_CURL_OFF_T " of %" CURL_FORMAT_CURL_OFF_T
             "\r\n",
             ulnow, ultotal, dlnow, dltotal);*/
    return 0;
}

/* for libcurl older than 7.32.0 (CURLOPT_PROGRESSFUNCTION) */
int Retriever::older_progress(void *p,
                              double dltotal, double dlnow,
                              double ultotal, double ulnow)
{
    return Retriever::xferinfo(p,
                               (curl_off_t)dltotal,
                               (curl_off_t)dlnow,
                               (curl_off_t)ultotal,
                               (curl_off_t)ulnow);
}

MemoryResource Retriever::get(const std::string &url)
{
    std::string mod_url = url;
    if (url.find("package://") == 0)
    {
        mod_url.erase(0, strlen("package://"));
        size_t pos = mod_url.find("/");
        if (pos == std::string::npos)
        {
            throw Exception(url, "Could not parse package:// format into file:// format");
        }

        std::string package = mod_url.substr(0, pos);
        mod_url.erase(0, pos);
        std::string package_path = ros::package::getPath(package);

        if (package_path.empty())
        {
            throw Exception(url, "Package [" + package + "] does not exist");
        }

        mod_url = "file://" + package_path + mod_url;
    }


    curl_easy_setopt(curl_handle_, CURLOPT_URL, mod_url.c_str());

    curl_easy_setopt(curl_handle_, CURLOPT_WRITEFUNCTION, curlWriteFunc);

    curl_easy_setopt(curl_handle_, CURLOPT_NOPROGRESS, 0L);

    char error_buffer[CURL_ERROR_SIZE];
    curl_easy_setopt(curl_handle_, CURLOPT_ERRORBUFFER , error_buffer);

    curl_easy_setopt(curl_handle_, CURLOPT_PROGRESSFUNCTION, &resource_retriever::Retriever::older_progress);
    /* pass the struct pointer into the progress function */
    prog_.last_in_secs = 0;
    prog_.curl = curl_handle_;
    prog_.dl_total = 0;
    prog_.dl_now = 0;
    curl_easy_setopt(curl_handle_, CURLOPT_PROGRESSDATA, &(prog_));

#if LIBCURL_VERSION_NUM >= 0x072000
    /* xferinfo was introduced in 7.32.0, no earlier libcurl versions will
    compile as they won't have the symbols around.

    If built with a newer libcurl, but running with an older libcurl:
    curl_easy_setopt() will fail in run-time trying to set the new
    callback, making the older callback get used.

    New libcurls will prefer the new callback and instead use that one even
    if both callbacks are set. */

    curl_easy_setopt(curl_handle_, CURLOPT_XFERINFOFUNCTION, &resource_retriever::Retriever::xferinfo);
    /* pass the struct pointer into the xferinfo function, note that this is
    an alias to CURLOPT_PROGRESSDATA */
    curl_easy_setopt(curl_handle_, CURLOPT_XFERINFODATA, &(prog_));
#endif

    curl_easy_setopt(curl_handle_, CURLOPT_NOPROGRESS, 0L);

    MemoryResource res;
    MemoryBuffer buf;
    curl_easy_setopt(curl_handle_, CURLOPT_WRITEDATA, &buf);

    CURLcode ret = curl_easy_perform(curl_handle_);
    if (ret != 0)
    {
        throw Exception(mod_url, error_buffer);
    }
    else if (!buf.v.empty())
    {
        res.size = buf.v.size();
        res.data.reset(new uint8_t[res.size]);
        memcpy(res.data.get(), &buf.v[0], res.size);
    }

    return res;
}

}
