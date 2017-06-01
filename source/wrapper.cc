
#include "wrapper.h"

#define JS_STR(str) Nan::New<v8::String>((str)).ToLocalChecked()
#define JS_UINT(num) Nan::New<v8::Uint32>((num))

#define JS_PROP(obj, prop) Nan::To<bool>(Nan::Get(obj, JS_STR(prop)).ToLocalChecked()).FromJust()

namespace ps {
namespace wrapper {

using std::runtime_error;

using native::Process;
using native::ProcessProperties;
using native::ProcessList;

using v8::Local;
using v8::Value;
using v8::Object;
using v8::Uint32;
using v8::Int32;
using v8::String;
using v8::Array;
using v8::Function;

using Nan::HandleScope;
using Nan::AsyncWorker;
using Nan::Callback;

namespace { // anonymous

} // anonymous namespace

void ListWorker::Execute()
{
    try {
        processes_ = native::List();
    }
    catch (const runtime_error& error) {
        SetErrorMessage(error.what());
    }
}

void ListWorker::HandleOKCallback()
{
    HandleScope scope;

    auto jsProcesses = Nan::New<Array>(static_cast<uint32_t>(processes_.size()));
    uint32_t index = 0;
    for (auto it = processes_.begin(); it != processes_.end(); ++it) {
        auto jsProcess = Nan::New<Object>();
        if (0 != (props_ & ProcessProperties::Pid))
            Nan::Set(jsProcess, JS_STR("pid"), JS_UINT(it->pid));
        if (0 != (props_ & ProcessProperties::Name)) 
            Nan::Set(jsProcess, JS_STR("name"), JS_STR(it->name));
        if (0 != (props_ & ProcessProperties::Parent))
            Nan::Set(jsProcess, JS_STR("parent"), JS_UINT(it->parent));
        if (0 != (props_ & ProcessProperties::Priority))
            Nan::Set(jsProcess, JS_STR("priority"), JS_UINT(it->priority));
        if (0 != (props_ & ProcessProperties::Threads))
            Nan::Set(jsProcess, JS_STR("threads"), JS_UINT(it->threads));

        Nan::Set(jsProcesses, index, jsProcess);
        index++;
    }

    const int argc = 2;
    Local<Value> argv[] = { Nan::Null(), jsProcesses };
    callback->Call(argc, argv);
}

void ListWorker::HandleErrorCallback()
{
    HandleScope scope;

    const int argc = 2;
    Local<Value> argv[] = { Nan::Error(ErrorMessage()), Nan::Null() };
    callback->Call(argc, argv);
}

void KillWorker::Execute()
{
    try {
        native::Kill(pid_, signal_);
    }
    catch (const runtime_error& error) {
        SetErrorMessage(error.what());
    }
}

void KillWorker::HandleOKCallback()
{
    HandleScope scope;

    const int argc = 1;
    Local<Value> argv[] = { Nan::Null() };
    callback->Call(argc, argv);
}

void KillWorker::HandleErrorCallback()
{
    HandleScope scope;

    const int argc = 1;
    Local<Value> argv[] = { Nan::Error(ErrorMessage()) };
    callback->Call(argc, argv);
}

NAN_METHOD(List)
{
    HandleScope scope;

    if (1 == info.Length() && 
             info[0]->IsFunction()) {
        auto callback = new Callback(info[0].As<Function>());
        Nan::AsyncQueueWorker(new ListWorker(callback, ProcessProperties::All));
    }
    else if (2 == info.Length() && 
                  info[0]->IsArray() &&
                  info[1]->IsFunction()) {
        auto options = info[0].As<Array>();
        auto callback = new Callback(info[1].As<Function>());

        uint8_t props = ProcessProperties::None;
        for (uint32_t i = 0; i < options->Length(); ++i) {
            const String::Utf8Value value(Nan::Get(options, i).ToLocalChecked().As<String>());
            if (0 == std::strcmp(*value, "pid")) props |= ProcessProperties::Pid;
            if (0 == std::strcmp(*value, "name")) props |= ProcessProperties::Name;
            if (0 == std::strcmp(*value, "parent")) props |= ProcessProperties::Parent;
            if (0 == std::strcmp(*value, "priority")) props |= ProcessProperties::Priority;
            if (0 == std::strcmp(*value, "threads")) props |= ProcessProperties::Threads;
        }
        
        Nan::AsyncQueueWorker(new ListWorker(callback, static_cast<ProcessProperties>(props)));
    }
    else {
        return Nan::ThrowTypeError("Invalid arguments");
    }
}

NAN_METHOD(Kill)
{
    HandleScope scope;

    if (2 == info.Length() &&
             info[0]->IsUint32() &&
             info[1]->IsFunction()) {
        uint32_t pid = Nan::To<uint32_t>(info[0].As<Uint32>()).FromJust();
        auto callback = new Callback(info[1].As<Function>());
        Nan::AsyncQueueWorker(new KillWorker(callback, pid));
    }
    else if (3 == info.Length() && 
                  info[0]->IsUint32() && 
                  info[1]->IsInt32() && 
                  info[2]->IsFunction()) {
        uint32_t pid = Nan::To<uint32_t>(info[0].As<Uint32>()).FromJust();
        int32_t signal = Nan::To<int32_t>(info[1].As<Int32>()).FromJust();
        auto callback = new Callback(info[2].As<Function>());
        Nan::AsyncQueueWorker(new KillWorker(callback, pid, signal));
    }
    else {
        return Nan::ThrowTypeError("Invalid arguments");
    }
}

} // wrapper namespace
} // ps namespace
