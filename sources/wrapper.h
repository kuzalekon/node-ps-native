
#pragma once

#ifndef PS_WRAPPER_H
#define PS_WRAPPER_H

#include "process.h"

#include <nan.h>

namespace ps {

class ListWorker final : public Nan::AsyncWorker
{
public:
    ListWorker(Nan::Callback* callback, Process::InfoSet infoset) 
        : Nan::AsyncWorker(callback)
        , infoset_(infoset) {};
    ~ListWorker() = default;

public:
    void Execute() override;

    void HandleOKCallback() override;
    void HandleErrorCallback() override;

private:
    Process::InfoSet infoset_;
    Process::List processes_;
}; // class ListWorker

class FindWorker final : public Nan::AsyncWorker
{
public:
    FindWorker(Nan::Callback* callback, uint32_t pid, Process::InfoSet infoset)
        : Nan::AsyncWorker(callback)
        , pid_(pid)
        , mask_("")
        , infoset_(infoset)
        , process_({ 0 }) {}
    FindWorker(Nan::Callback* callback, const std::string& mask, Process::InfoSet infoset)
        : Nan::AsyncWorker(callback)
        , pid_(0)
        , mask_(mask)
        , infoset_(infoset)
        , process_({ 0 }) {}
    ~FindWorker() = default;

public:
    void Execute() override;

    void HandleOKCallback() override;
    void HandleErrorCallback() override;

private:
    uint32_t pid_;
    std::string mask_;

    Process::InfoSet infoset_;
    Process::Info process_;
    Process::List processes_;
}; // class FindWorker

class KillWorker final : public Nan::AsyncWorker
{
public:
    KillWorker(Nan::Callback* callback, uint32_t pid, int32_t  code)
        : Nan::AsyncWorker(callback)
        , pid_(pid)
        , code_(code) {}
    ~KillWorker() = default;

public:
    void Execute() override;

    void HandleOKCallback() override;
    void HandleErrorCallback() override;

private:
    uint32_t pid_;
    int32_t code_;
}; // class KillWorker

class Wrapper
{
public:
    Wrapper() = default;
    Wrapper(const Wrapper& other) = delete;

public:
    Wrapper& operator=(const Wrapper& rhs) = delete;

public:
    static NAN_MODULE_INIT(Initialize);

private:
    static NAN_METHOD(Enum);
    static NAN_METHOD(Find);
    static NAN_METHOD(Kill);
}; // class Wrapper

} // ps namespace

#endif // PS_WRAPPER_H