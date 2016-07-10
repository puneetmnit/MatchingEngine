//
// Created by puneet on 09/07/16.
//

#include "SimpleMatcher.h"

void SimpleMatcher::async_reply(int order_id, const ResponseCallbackT& callback)
{
    if (callback) {
        std::lock_guard<std::mutex> guard{replyLock_};
        replies_.emplace_back(callback, order_id);
    }

};

SimpleMatcher::~SimpleMatcher()
{
    //wait to all the async-replies to be sent
    for(auto& t : replies_) {
        if (t.joinable()) t.join();
    }
}



