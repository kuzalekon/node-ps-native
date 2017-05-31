
#pragma once

#ifndef PS_WRAPPER_H
#define PS_WRAPPER_H

#include "process.h"

#include <nan.h>

namespace ps {
namespace wrapper {

class EnumWorker final : public Nan::AsyncWorker
{
public:
    EnumWorker(Nan::Callback* callback) : Nan::AsyncWorker(callback) {};
    ~EnumWorker() {};

    void Execute() override;

    void HandleOKCallback() override;
    void HandleErrorCallback() override;

private:
    native::ProcessList processes_;
};

NAN_METHOD(List);

} // wrapper namespace
} // ps namesapce

#endif // PS_WRAPPER_H