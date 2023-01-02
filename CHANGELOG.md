yasio-3.39.7
  
  1. Add forward event support fire packet event, new option: `YOPT_S_FORWARD_EVENT`,  after enable forward event: 

      - Use `event->packet_view()` receive packet event
      - No upack
      - No deferred event
  2. Refactor ssl backends, add follow new option and channel kind
 
      - `YOPT_S_SSL_CERT`: to specific ssl server `cert` and `private_key` files
      - `YCK_SSL_SERVER`: open a channel as ssl server, notes: require specific valid `cert` and `private_key` file with option `YOPT_S_SSL_CERT`
  
  
yasio-3.39.6
  
  1. Reimplement reactor backends: select, poll
  2. Now the default reactor backend is poll
  3. Add compiler flag `YASIO_DISABLE_POLL` for switching to socket.select as same with previous releases
  4. Add `signal_blocker` when create service thread
  
  
yasio-3.39.5
  
  1. Fix no callback when resolve domain failed by `getaddrinfo`


yasio-3.39.4
  
  1. Fix process order
  2. Fix luabinding compile issue when shared packet enabled
  3. Improve CMake script, output libs to same directory
  
  
yasio-3.39.3
  
  1. Improve `byte_buffer` stl compatible
  2. Add option `YOPT_C_UNPACK_NO_BSWAP`
  3. Add `urlEncode/urlDecode` to extension `yasio_http`
  4. Improve extension `yasio_http` as unreal engine plugin
  5. Fix missing retrun statement in `cxx17::string_view` hasher template
  6. Change lua script io_event.packet(bool) to io_event:packet(bufferType)
      - yasio.BUFFER_DEFAULT: take packet as native yasio::ibstream
      - yasio.BUFFER_NO_BSWAP: take packet as natvie yasio::fast_ibstream without byte order convert
      - yasio.BUFFER_RAW: take packet as lua string
  7. CMake: Set default ssl backend to mbedtls
  
  
yasio-3.39.2
  
  1. Use `byte_buffer` instead `std::vector<char>`
  2. Add extension `yasio_http` implementation
  3. Fix name query will defunct when name server dirty
  
  
yasio-3.39.1
  
1. Don't output transport object address
  
  
yasio-3.39.0
  
1. Improve binary reader and writer, the push/pop API was broken, use -DYASIO_OBS_BUILTIN_STACK=1 for compatible with previous release
2. Improve stop flow, always try do stop at io_service destructor
  
  
yasio-3.37.8
  
1. **Fix trasnport reuse problem**
2. Unify transport address print format
  
  
yasio-3.37.7
  
1. Fix connnect maybe always failed with error `-26` when last domain name resolving failed
2. Fix `xxsocket::strerror` incorrect for `MinGW`
3. Add `UWP` ci
4. Improve io_service::stop flow, thanks to **@koobin**
5. Other code improvements
  
  
yasio-3.37.6
  
1. Add option `YOPT_S_DNS_LIST` to set custom dns servers build with c-ares
2. Add OpenSSL 3.0.0 support
3. Add better API `ip::endpoint::format_to` to format socket address to string
4. Add API `ip::endpoint::scope_id` to set or get ipv6 `scope_id`
5. Improve `ip::endpoint::as_xx` for parsing `scope_id` from ipv6 string
6. Add MinGW build support
7. Fix compile issue when build inside unreal engine with module `TraceAnalysis`
8. Fix timer not sort when same timer expire time changed, thanks to `@koobin`
9. Fix the `len` of `ip::endpoint` not set when update endpoint ip with `ip::endpoint::ip`, will cause error `10014` on windows
10. Make `ibstream_view` can construct with `basic_obstream_view`
11. Improve `xxsocket::disconnect` platform compatible for BSD-like systems and windows
12. Imporve `multicast` support, and the test case works on windows,linux,macOS, thanks to **@wzhengsen**
13. Add option `YOPT_C_MCAST_IF` to set multicast interface, on BSD-like system, it's required for ipv6
14. Improve `io_service::stop` flow, thanks to **@koobin**
  
  
yasio-3.37.5
  
1. Make `yasio::obstream` more reusable
2. Fix multicast behavior incorrect
3. Fix ssl handle not destroy when connect timeout reached
4. Mbedtls 3.0.0 support
  
  
yasio-3.37.4
  
1. Make c++20 compatible API `starts_with/ends_wtih` more easy to use
2. Correct `yasio::set_thread_name` platform macro detection
3. Move `yasio::errc` to standalone file `errc.hpp`
4. Improve `YOPT_DNS_DIRTY` behavior
5. Fix header only link when c-ares enabled
6. Make `yasio::strfmt` more reusable
7. Improve Unreal Engine support
  
  
yasio-3.37.3
  
1. Unify io_service timeout options: `XXX_TIMEOUT in seconds` and `XXX_TIMEOUTMS in milliseconds`
2. Add io_channel user timer support
3. Move `yasio:errc` to `yasio.hpp`
4. Add `yasio_fwd.hpp`
  
  
yasio-3.37.2
  
1. Add pkg-config file.
2. Improve UnrealEngine support.
3. Improve cmake scripts.
4. Fix bind failed not processed.
5. Add passive event support for server open/close.
6. Add YASIO_ENABLE_PASSIVE_EVENT to control whether generate passive event.
7. Add github action docs-ci and host docs on github pages.
8. Fix gcc-4.7 compile issues.
9. Renable appveyor ci for build with vs2013.
10. Inline namespace `yasio::inet`.
  
  
yasio-3.37.1
  
1. Update yasio_ni to v2.
2. Add `io_channel::bytes_transferred`, it's useful for client channel `Traffic Statistics`.
3. Add `io_channel::connect_id`.
4. Add support build as windows dll.
5. Add `YASIO_USE_SHARED_PACKET` to control whether packet could be shared on multi-threadings.
  
  
yasio-3.37.0
  
1. Make timer object more safe, don't hold reference of io_service.
2. Embed default zh_CN docs markdown sources.
3. Fix compile error when YASIO_VERBOSE_LOG enabled.
4. Fix `speedtest` compile error when YASIO_HAVE_KCP enabled.
5. Make `host_to_network` and `network_to_host` public at `namespace yasio`.
6. Add `yasio::set_thread_name` to set caller thread name.
7. Add `obstream::clear` for buffer reuse.
8. Add `ibstream::advance` to move read position fastly.
9. Enhance builtin `decode_len` function for transport stream unpack.
10. Add github action `freebsd`.
  
  
yasio-3.36.0
  
1. Add ssl backend mbedtls support.
2. Add `xxsocket::not_send_error` for check whether socket status is good when send retval < 0.
3. Add `xxsocket::not_recv_error` for check whether socket status is good when recv retval < 0.
4. Delete `xxsocket::alive`.
5. Delete all deprecated functions.
6. Rename `xxsocket::detach` to `xxsocket::release_handle`.
7. Rename `xxsocket::pserv` to `xxsocket::pserve`.
  
  
yasio-3.35.0
  
- Provides normally byte order convert function templates `host_to_network` and `network_to_host` at `namespace yasio::endian`.
- Reimplement `obstream`, `ibstream` as class templates `basic_obstream`, `basic_ibstream` with convert_traits support.
- Using new `yasio::endian::convert_traits<network_convert_tag>` for `obstream` and `ibstream` with byte order convertion.
- Using new `yasio::endian::convert_traits<host_convert_tag>` for `fast_obstream` and `fast_ibstream` without byte order convertion.
- Add io_service::cleanup_globals  
*It's useful to clear custom print function object when you unload the module
(usually .dll or .so) which contains the function object.*
- Fix socket reuse_address behavior since v3.33.7
- Add YOPT_S_DEFER_EVENT_CB.  
  *a. User can do custom packet resolve at network thread, such as decompress and crc check.*  
  *b. Return true, io_service will continue enque to event queue.*  
  *c. Return false, io_service will drop the event.*  
  *d. Callback prototype is: typedef std::function<bool(event_ptr& event)> defer_event_cb;*
  
  
yasio-3.34.0
  
- Add ```7bit Encoded Int64``` support for **obstream/ibstream**
- Rename **obstream/ibstream** ```7bit Encoded Int``` APIs ```write_i/read_i``` to ```write_ix/read_ix```
- Rename **obstream/ibstream** ```Fixed Encoded Number``` APIs ```write_ix/read_ix``` to ```write/read```
  
  
yasio-3.33.9
  
- Fix win32 udp server implementation
- Print remote point as peer for a new udp transport
- Remove options ```YOPT_IGNORE_UDP_ERROR```
  
  
yasio-3.33.8
  
- Auto call socket.shutdown at xxsocket::close to fix blocking on socket.recv when close socket at other thread for non-win32 platforms.
  
  
yasio-3.33.7
  
- Unix domain socket via ```SOCK_STREAM``` support, define ```YASIO_ENABLE_UDS``` at ```config.hpp``` and use ```YCM_UDS``` combine with ```YCK_TCP_CLIENT```, ```YCK_TCP_SERVER```
  
  
yaiso-3.33.6
  
- Reduce kcp cpu cost.
  
  
yaiso-3.33.5
  
- Add support to set kcp conv id, use option ```YOPT_C_KCP_CONV```.
  
  
yaiso-3.33.4
  
- Add log level support, use option ```YOPT_S_PRINT_FN2```.
  
  
yaiso-3.33.3
  
- Add API ```xxsocket::tcp_rtt```.
  
  
yaiso-3.33.2
  
- Fix c-ares doesn't get system dns for ios.
- Add option ```YOPT_S_DNS_DIRTY``` for user to change system name servers after mobile network changed when c-ares enabled.
- Refine write event register when system kernel write buffer is full.
  
  
yaiso-3.33.1
  
  
- Reduce the size of the Windows header files.
- Improve yasio::inet::ip::endpoint code style.
- Explicit socket_select_interrupter workaround code logic for borken firewalls on Windows.
- Sets and pass user data of transport through event.
- Fix KCP do_read may can't dispatch upper data to user.
- Fix behavior of yasio::wcsfmt.
  
  
yasio-3.33.0
  
- Refactor UDP like transport, UDP client don't establish 4-tuple with peer, and provide  ```YPOT_T_CONNECT``` and ```YPOT_T_DISCONNECT``` to change association.
- Add ```io_service::write_to``` for ```unconnected/connected``` UDP transport.
- Remove unused channel masks ```YCM_MCAST_CLIENT```, ```YCM_MCAST_SERVER```
- Remove unused channel flag ```YCF_MCAST_LOOPBACK```
- Add new options ```YOPT_C_ENABLE_MCAST```, ```YOPT_C_DISABLE_MCAST``` for multicast support
- Change ```timer_cb_t``` prototype to ```[]()->bool { }```, return ```true``` for once, ```false``` for continue.
- Add ```highp_timer::async_wait_once``` to wait timer timeout once.
- Change ```YCM_XXX_[CLIENT/SERVER]``` to ```YCK_XXX_[CLIENT/SERVER]```.
- Add ```yasio::xhighp_clock``` to retrive nanoseconds timestamp.
- Fix xxsocket APIs connect_n, recv_n doesn't handle signal EINTR.
- Tidy obstream/ibstream API, by default ```write_v/read_v``` use ```7bit encoded int``` for length field.
- Rename io_service ```start_service/stop_service``` to ```start/stop```.
- Fix c-ares timeout behavior.
- Improve c-ares cleanup behavior, now destruct io_service more stable with c-ares enabled.
- Make ```cxx17::string_view``` support unordered set/map on compilers which only support c++11 standard.
- Use ```shared_ptr + shared_mutex``` to ensure destruct io_service safe without side affect for concurrency of name resolving.
- Fix dns cache timeout mechanism doesn't work.
- Simplify c-ares dns-server setup on android, when use yasio as static library, you need call ```yasio__jni_onload``` at ```JNI_OnLoad```.
- Fix ```yasio::_strfmt``` may crash on some incorrect use, now it's more stable on all platforms.
- Improve behavior when kernel send buffer is full, don't sleep a fixed time, just drived by ```select```.
- Optimize udp transport close behavior, by default, udp transport will never close except user request, still can use io_service's option YOPT_S_IGNORE_UDP_ERROR to change this behavior.
- Change write completion handler prototype to: ```std::function<void(int ec, size_t bytes_transferred)>```.
- Implement literals for ```cxx17::string_view```.
- Fix ssl handshake failed with certificate verify failed when cacert file provided and flag ```SSL_VERIFY_PEER``` was set.
- Fix doesn't call io_service destructor when use lua binding library ```kaguya``` on compiler without c++14 support.
- Add ```io_service::init_globals(const print_fn_t&)``` to support redirect initialization log to custom file(U3D/UE4 Console).
- Improve compiler support, now support c++14, c++17, c++20.
- Auto choose library ```sol2``` for lua binding when ```cxx_std >= 14```, older require ```cxx_std >= 17```.
- Recreate the socket_select_interrupter's sockets on error. 
- Update kcp to v1.7, the kcp older version may cause SIGBUS on mobile ARM.
- Simplify API, remove unnecessary API ```io_service::reopen```, please use ```io_service::open``` instead.
- Fix crash at yasio::inet::ip::endpoint::ip() when af=0.
- Make io_service::write to a kcp return value same as other channel.
- Fix kcp server doesn't decode packet header.
- Add xxsocket::disconnect to dissolve the 4-tuple association.
- Rename option ```YOPT_I_SOCKOPT``` to ```YOPT_B_SOCKOPT```.
- Other code quality & stable improvements.
  
yasio-3.31.3
  
- Optimize API io_service::write, add write raw buf support.
- Fix issue: https://github.com/yasio/yasio/issues/208
- Fix issue: https://github.com/yasio/yasio/issues/209
  
  
yasio-3.31.2
  
- Optimize singleton implementation, see: https://github.com/yasio/yasio/issues/200
- Fix typo, YASIO_VERBOS_LOG to YASIO_VERBOSE_LOG.
- Explicit set socktype for ```getaddrinfo```, see: https://github.com/yasio/yasio/issues/201
- Improve xxsocket send_n/recv_n implementation and behavior, see: https://github.com/yasio/yasio/issues/202
  
  
yasio-3.31.1
  
- Make c-ares works well on android 8 or later.
  
  
yasio-3.31.0
  
- Add Initial Bytes To Strip support for length field based frame decode, use YOPT_C_LFBFD_IBTS.
- Add SSL client support, use YASIO_HAVE_SSL to enable, YCM_SSL_CLIENT to open a channel with ssl client support.
- Integrate c-ares support, YASIO_HAVE_CARES to enable, make sure your build system already have c-ares.
- Refactor timeout options, use YOPT_S_CONNECT_TIMEOUT, YOPT_S_DNS_CACHE_TIMEOUT, YOPT_S_DNS_QUERIES_TIMEOUT to instead YOPT_S_TIMEOUTS.
- Optimize schedule_timer behavior, always replace timer_cb when timer exist.
- Remove deprecated functions.
- Remove tolua support.
  
  
yasio-3.30.8
  
- Sync optimizations from v3.31.2.
  
  
yasio-3.30.7
  
- Sync optimizations from v3.31.
  
  
yasio-3.30.6
  
- Make option enums compatible with v3.31.
- Simplify io_service state.
  
  
yasio-3.30.5
  
- Fix missing break at set_option for YOPT_C_REMOTE_ENDPOINT.
  
  
yasio-3.30.4
  
- Make normal concurrent queue more safe if SPSC queue is disabled.
- Make sure udp recv buf enough at kcp transport do_read.
- Fix io_service::write always retur 0.
  
  
yasio-3.30.3
  
- Optimize jsb, jsb2.0 and lua binding.
  
  
yasio-3.30.2
  
- Fix jsb2.0 bindings issue, see: [#192](https://github.com/yasio/yasio/issues/192)
  
  
yasio-3.30.1
  
- Add construct channel at io_service constructor, now all options could be set before start_service if you doesn't use the start_service with channel hostents, and mark them deprecated
- Use cmake for trvis ci cross-platform build at win32, linux, osx, ios
- Sync script bindings
  
  
yasio-3.30.0
  
- Tidy option macros.
- Add YCF_ enums to control channel to support more features.
- Add multicast support.
- Add a workaround implementation to support win32 udp-server.
- Add io_service::cindex_to_handle.
- Add ftp sever example.
- Remove loop behavior of deadline_timer, user can schedule again when it's expired.
- Add obstream::write_byte.
- Add to_strf_v4 for ip::endpoint.
- Optimizing for file transfer, avoid high cpu occupation when system kernel send buffer is full.
- More safe to check object valid which allocated from pool.
- Add send complete callback.
- Mark io_service::dispatch_events deprecated, use dispatch to instead.
- Add YCF_REUSEPORT to control whether to enable socket can bind same port, default and previous vesion is enabled.
- Implement case insensitive starts_with, ends_with at string_view.hpp.
- **Ignore SIGPIPE for tcp at non-win32 system.**
- **Remove reconnect timeout.**
