
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
}; // class Wrapper

} // ps namespace

#endif // PS_WRAPPER_H