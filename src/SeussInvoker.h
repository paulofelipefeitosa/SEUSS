//          Copyright Boston University SESA Group 2013 - 2018.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)
#ifndef SEUSS_INVOKER_H
#define SEUSS_INVOKER_H

#ifndef __ebbrt__
#error THIS IS EBBRT-NATIVE CODE
#endif

#include <ebbrt/Debug.h>
#include <ebbrt/Future.h>
#include <ebbrt/MulticoreEbb.h>
#include <ebbrt/native/Net.h>
#include <ebbrt/native/NetTcpHandler.h>

#include "umm/src/Umm.h"

namespace seuss {

void Init();

class InvocationSession : public ebbrt::TcpHandler {
public:
  InvocationSession(ebbrt::NetworkManager::TcpPcb pcb, std::string args,
                    std::string code, uint64_t tid)
      : ebbrt::TcpHandler(std::move(pcb)), function_code_(code),
        run_args_(args), run_id_(tid) {
    once_connected = set_connected_.GetFuture();
  }
  /* TCP connection aborted */ 
  void Abort();
  /* TCP connection closed */ 
  void Close();
  /* TCP connection established */ 
  void Connected();
  /* Send HTTP request */ 
  void SendHttpRequest(std::string path, std::string payload);
  /* TCP connection receive data */ 
  void Receive(std::unique_ptr<ebbrt::MutIOBuf> b);
  ebbrt::Future<void> once_connected;

private:
  std::string http_post_request(std::string path, std::string payload);
  std::string function_code_;
  ebbrt::Promise<void> set_connected_;
  bool is_connected_{false};
  bool is_initialized_{false};
  // RUN specific state
  std::string run_args_;
  uint64_t run_id_{0};
}; // class InvocationSession

/*  suess::Invoker 
 *  Per-core ebb responsible for initializing the
 *  UM instances, executing the function code and, eventually, caching and
 *  redeploying instance snapshots.
 */
class Invoker : public ebbrt::MulticoreEbb<Invoker> {
public:
  static const ebbrt::EbbId global_id = ebbrt::GenerateStaticEbbId("Invoker");
  Invoker(){};
  /* Bootstrap the instance on this core */
  void Bootstrap();
  /* Invoke code on an uninitialized instance */
  void Invoke(uint64_t tid, size_t fid, const std::string args,
              const std::string code);
  // Resolve invocation request
  void Resolve(uint64_t tid, const std::string ret);

private:
  bool is_bootstrapped_{false}; // Have we created a base snapshot
  bool is_running_{false};      // Have we booted the snapshot

  // Session specific state 
  size_t fid_{0};
  InvocationSession *umsesh_{nullptr};

  // TODO: FIXME: XXX: locking...
  std::mutex m_;
  typedef std::pair<std::string, std::string> invocation_request;
  std::unordered_map<uint64_t, invocation_request> request_map_;
  std::queue<uint64_t> request_queue_;
  std::unordered_map<uint32_t, ebbrt::Promise<void>> promise_map_;
  // TODO: move sv into a root Ebb?
  umm::UmSV base_um_env_;
};

constexpr auto invoker = ebbrt::EbbRef<Invoker>(Invoker::global_id);

} // namespace seuss

#endif // SEUSS_INVOKER
