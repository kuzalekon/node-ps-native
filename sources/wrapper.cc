
#include "wrapper.h"

#include <cstring>

#define JS_STR(str) Nan::New<v8::String>((str)).ToLocalChecked()
#define JS_UINT(num) Nan::New<v8::Uint32>((num))

#define JS_PROP(obj, prop) Nan::To<bool>(Nan::Get(obj, JS_STR(prop)).ToLocalChecked()).FromJust()

namespace ps {

using std::runtime_error;

using v8::Local;
using v8::Value;
using v8::Object;
using v8::Uint32;
using v8::Int32;
using v8::String;
using v8::Array;
using v8::Function;

using Nan::Callback;

void ListWorker::Execute()
{
    try {
        processes_ = Process::Enum(infoset_);
    }
    catch (const runtime_error& error) {
        SetErrorMessage(error.what());
    }
}

void ListWorker::HandleOKCallback()
{
    auto jsProcesses = Nan::New<Array>(static_cast<uint32_t>(processes_.size()));
    uint32_t index = 0;
    for (auto process : processes_) {
        auto jsProcess = Nan::New<Object>();
        if (0 != (infoset_ & Process::Pid))
            Nan::Set(jsProcess, JS_STR(u8"pid"), JS_UINT(process.pid));
        if (0 != (infoset_ & Process::Parent))
            Nan::Set(jsProcess, JS_STR(u8"parent"), JS_UINT(process.parent));
        if (0 != (infoset_ & Process::Name))
            Nan::Set(jsProcess, JS_STR(u8"name"), JS_STR(process.name));
        if (0 != (infoset_ & Process::Priority))
            Nan::Set(jsProcess, JS_STR(u8"priority"), JS_UINT(process.priority));
        if (0 != (infoset_ & Process::Threads))
            Nan::Set(jsProcess, JS_STR(u8"threads"), JS_UINT(process.threads));

        Nan::Set(jsProcesses, index, jsProcess);
        index++;
    }

    const int argc = 2;
    Local<Value> argv[argc] = { Nan::Null(), jsProcesses };
    callback->Call(argc, argv);
}

void ListWorker::HandleErrorCallback()
{
    const int argc = 2;
    Local<Value> argv[argc] = { Nan::Error(ErrorMessage()), Nan::Null() };
    callback->Call(argc, argv);
}

NAN_MODULE_INIT(Wrapper::Initialize)
{
    Nan::Export(target, u8"enum", Enum);
}

NAN_METHOD(Wrapper::Enum)
{
    if (1 == info.Length() && 
             info[0]->IsFunction()) {
        auto callback = new Callback(info[0].As<Function>());
        Nan::AsyncQueueWorker(new ListWorker(callback, Process::FullInfo));
    }
    else if (2 == info.Length() && 
                  info[0]->IsArray() &&
                  info[1]->IsFunction()) {
        auto jsinfoset = info[0].As<Array>();
        auto callback = new Callback(info[1].As<Function>());

        uint8_t infoset = Process::EmptyInfo;
        for (uint32_t i = 0; i < jsinfoset->Length(); ++i) {
            const String::Utf8Value value(Nan::Get(jsinfoset, i).ToLocalChecked());
            if (0 == _strcmpi(*value, u8"pid")) infoset |= Process::Pid;
            if (0 == _strcmpi(*value, u8"parent")) infoset |= Process::Parent;
            if (0 == _strcmpi(*value, u8"name")) infoset |= Process::Name;
            if (0 == _strcmpi(*value, u8"priority")) infoset |= Process::Priority;
            if (0 == _strcmpi(*value, u8"threads")) infoset |= Process::Threads;
        }
        
        Nan::AsyncQueueWorker(new ListWorker(callback, static_cast<Process::InfoSet>(infoset)));
    }
    else {
        Nan::ThrowTypeError(u8"Invalid arguments");
    }
}

} // ps namespace
