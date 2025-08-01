/** Available items from url protocols
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */
#include "config.h"
#include "config_components.h"
static const URLProtocol *url_protocols[] = {
    &ff_async_protocol,
    &ff_cache_protocol,
    &ff_concat_protocol,
    &ff_concatf_protocol,
    &ff_crypto_protocol,
    &ff_data_protocol,
    &ff_fd_protocol,
    &ff_ffrtmphttp_protocol,
    &ff_file_protocol,
    &ff_ftp_protocol,
    &ff_gopher_protocol,
#if CONFIG_GOPHERS_PROTOCOL
    &ff_gophers_protocol,
#endif
    &ff_hls_protocol,
    &ff_http_protocol,
    &ff_httpproxy_protocol,
#if CONFIG_HTTPS_PROTOCOL
    &ff_https_protocol,
#endif
    &ff_icecast_protocol,
    &ff_mmsh_protocol,
    &ff_mmst_protocol,
    &ff_md5_protocol,
    &ff_pipe_protocol,
    &ff_prompeg_protocol,
    &ff_rtmp_protocol,
#if CONFIG_RTMPS_PROTOCOL
    &ff_rtmps_protocol,
#endif
    &ff_rtmpt_protocol,
#if CONFIG_RTMPTS_PROTOCOL
    &ff_rtmpts_protocol,
#endif
    &ff_rtp_protocol,
    &ff_srtp_protocol,
    &ff_subfile_protocol,
    &ff_tee_protocol,
    &ff_tcp_protocol,
#if CONFIG_TLS_PROTOCOL
    &ff_tls_protocol,
#endif
    &ff_udp_protocol,
    &ff_udplite_protocol,
#if CONFIG_IPFS_GATEWAY_PROTOCOL
    &ff_ipfs_gateway_protocol,
#endif
#if CONFIG_IPNS_GATEWAY_PROTOCOL
    &ff_ipns_gateway_protocol,
#endif
    NULL };