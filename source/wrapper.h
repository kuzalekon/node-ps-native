
#pragma once

#ifndef PS_WRAPPER_H
#define PS_WRAPPER_H

#include "process.h"

#include <nan.h>

namespace ps {
namespace wrapper {

class ListWorker final : public Nan::AsyncWorker
{
public:
    ListWorker(Nan::Callback* callback) : Nan::AsyncWorker(callback) {};
    ~ListWorker() = default;

    void Execute() override;

    void HandleOKCallback() override;
    void HandleErrorCallback() override;

private:
    native::ProcessList processes_;
}; // ListWorker class

class KillWorker final : public Nan::AsyncWorker
{
public:
    KillWorker(Nan::Callback* callback, uint32_t pid, int32_t code, uint32_t timeout) 
        : Nan::AsyncWorker(callback)
        , pid_(pid)
        , code_(code)
        , timeout_(timeout) {};
    ~KillWorker() = default;

    void Execute() override;

    void HandleOKCallback() override;
    void HandleErrorCallback() override;

private:
    uint32_t pid_;
    int32_t code_;
    uint32_t timeout_;
}; // KillWorker class

NAN_METHOD(List);
NAN_METHOD(Kill);

} // wrapper namespace
} // ps namesapce

#endif // PS_WRAPPER_H