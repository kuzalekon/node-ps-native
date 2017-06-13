
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
using v8::RegExp;

using Nan::Callback;

namespace {

Local<Object> PackProcessInfo(const Process::Info& process, Process::InfoSet infoset)
{
    auto jsprocess = Nan::New<Object>();
    if (0 != (infoset & Process::Pid))
        Nan::Set(jsprocess, JS_STR(u8"pid"), JS_UINT(process.pid));
    if (0 != (infoset & Process::Parent))
        Nan::Set(jsprocess, JS_STR(u8"parent"), JS_UINT(process.parent));
    if (0 != (infoset & Process::Name))
        Nan::Set(jsprocess, JS_STR(u8"name"), JS_STR(process.name));
    if (0 != (infoset & Process::Priority))
        Nan::Set(jsprocess, JS_STR(u8"priority"), JS_UINT(process.priority));
    if (0 != (infoset & Process::Threads))
        Nan::Set(jsprocess, JS_STR(u8"threads"), JS_UINT(process.threads));

    return jsprocess;
}

Local<Array> PackProcessesList(const Process::List& processes, Process::InfoSet infoset)
{
    auto jsprocesses = Nan::New<Array>(static_cast<uint32_t>(processes.size()));
    uint32_t index = 0;
    for (auto process : processes) {
        auto jsprocess = Nan::New<Object>();
        if (0 != (infoset & Process::Pid))
            Nan::Set(jsprocess, JS_STR(u8"pid"), JS_UINT(process.pid));
        if (0 != (infoset & Process::Parent))
            Nan::Set(jsprocess, JS_STR(u8"parent"), JS_UINT(process.parent));
        if (0 != (infoset & Process::Name))
            Nan::Set(jsprocess, JS_STR(u8"name"), JS_STR(process.name));
        if (0 != (infoset & Process::Priority))
            Nan::Set(jsprocess, JS_STR(u8"priority"), JS_UINT(process.priority));
        if (0 != (infoset & Process::Threads))
            Nan::Set(jsprocess, JS_STR(u8"threads"), JS_UINT(process.threads));

        Nan::Set(jsprocesses, index, jsprocess);
        index++;
    }

    return jsprocesses;
}

uint8_t UnpackInfoSet(const Local<Array>& jsinfoset)
{
    uint8_t infoset = Process::EmptyInfo;
    for (uint32_t i = 0; i < jsinfoset->Length(); ++i) {
        const String::Utf8Value value(Nan::Get(jsinfoset, i).ToLocalChecked());
        if (0 == std::strcmp(*value, u8"pid")) infoset |= Process::Pid;
        if (0 == std::strcmp(*value, u8"parent")) infoset |= Process::Parent;
        if (0 == std::strcmp(*value, u8"name")) infoset |= Process::Name;
        if (0 == std::strcmp(*value, u8"priority")) infoset |= Process::Priority;
        if (0 == std::strcmp(*value, u8"threads")) infoset |= Process::Threads;
    }

    return infoset;
}

} // anonymous namespace

/*
 * ListWorker
 */

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
    const int argc = 2;
    Local<Value> argv[argc] = { Nan::Null(), PackProcessesList(processes_, infoset_) };
    callback->Call(argc, argv);
}

void ListWorker::HandleErrorCallback()
{
    const int argc = 2;
    Local<Value> argv[argc] = { Nan::Error(ErrorMessage()), Nan::Null() };
    callback->Call(argc, argv);
}

/*
* FindWorker
*/

void FindWorker::Execute()
{
    try {
        0 == pid_ ? Process::Find(mask_, processes_, infoset_)
                  : Process::Find(pid_, process_, infoset_);
    }
    catch (const runtime_error& error) {
        SetErrorMessage(error.what());
    }
}

void FindWorker::HandleOKCallback()
{
    const int argc = 2;
    Local<Value> argv[argc] = { Nan::Null() };
    0 == pid_ ? argv[1] = PackProcessesList(processes_, infoset_)
              : argv[1] = PackProcessInfo(process_, infoset_);
    callback->Call(argc, argv);
}

void FindWorker::HandleErrorCallback()
{
    const int argc = 2;
    Local<Value> argv[argc] = { Nan::Error(ErrorMessage()), Nan::Null() };
    callback->Call(argc, argv);
}

/*
 * KillWorker
 */

void KillWorker::Execute()
{
    try {
        Process::Kill(pid_, code_);
    }
    catch (const runtime_error& error) {
        SetErrorMessage(error.what());
    }
}

void KillWorker::HandleOKCallback()
{
    const int argc = 1;
    Local<Value> argv[argc] = { Nan::Null() };
    callback->Call(argc, argv);
}

void KillWorker::HandleErrorCallback()
{
    const int argc = 1;
    Local<Value> argv[argc] = { Nan::Error(ErrorMessage()) };
    callback->Call(argc, argv);
}

/*
 * Wrapper
 */

NAN_MODULE_INIT(Wrapper::Initialize)
{
    Nan::Export(target, u8"enum", Enum);
    Nan::Export(target, u8"find", Find);
    Nan::Export(target, u8"kill", Kill);
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
        uint8_t infoset = UnpackInfoSet(info[0].As<Array>());
        auto callback = new Callback(info[1].As<Function>());
        Nan::AsyncQueueWorker(new ListWorker(callback, static_cast<Process::InfoSet>(infoset)));
    }
    else {
        Nan::ThrowTypeError(u8"Invalid arguments");
    }
}

NAN_METHOD(Wrapper::Find)
{
    if (2 == info.Length() &&
             info[0]->IsUint32() &&
             info[1]->IsFunction()) {
        uint32_t pid = info[0]->Uint32Value();
        auto callback = new Callback(info[1].As<Function>());
        Nan::AsyncQueueWorker(new FindWorker(callback, pid, Process::FullInfo));
    }
    if (3 == info.Length() &&
             info[0]->IsUint32() &&
             info[1]->IsArray() &&
             info[2]->IsFunction()) {
        uint32_t pid = info[0]->Uint32Value();
        uint8_t infoset = UnpackInfoSet(info[1].As<Array>());
        auto callback = new Callback(info[2].As<Function>());
        Nan::AsyncQueueWorker(new FindWorker(callback, pid, static_cast<Process::InfoSet>(infoset)));
    }
    else if (2 == info.Length() &&
                  info[0]->IsString() &&
                  info[1]->IsFunction()) {
        const String::Utf8Value mask(info[0]);
        auto callback = new Callback(info[1].As<Function>());
        Nan::AsyncQueueWorker(new FindWorker(callback, *mask, Process::FullInfo));
    }
    else if (3 == info.Length() &&
                  info[0]->IsString() &&
                  info[1]->IsArray() &&
                  info[2]->IsFunction()) {
        const String::Utf8Value mask(info[0]);
        uint8_t infoset = UnpackInfoSet(info[1].As<Array>());
        auto callback = new Callback(info[2].As<Function>());
        Nan::AsyncQueueWorker(new FindWorker(callback, *mask, static_cast<Process::InfoSet>(infoset)));
    }
    else if (2 == info.Length() &&
                  info[0]->IsRegExp() &&
                  info[1]->IsFunction()) {
        const String::Utf8Value mask(info[0].As<RegExp>()->GetSource());
        auto callback = new Callback(info[1].As<Function>());
        Nan::AsyncQueueWorker(new FindWorker(callback, *mask, Process::FullInfo));
    }
    else if (3 == info.Length() &&
                  info[0]->IsRegExp() &&
                  info[1]->IsArray() &&
                  info[2]->IsFunction()) {
        const String::Utf8Value mask(info[0].As<RegExp>()->GetSource());
        uint8_t infoset = UnpackInfoSet(info[1].As<Array>());
        auto callback = new Callback(info[2].As<Function>());
        Nan::AsyncQueueWorker(new FindWorker(callback, *mask, static_cast<Process::InfoSet>(infoset)));
    }
    else {
        Nan::ThrowTypeError(u8"Invalid arguments");
    }
}

NAN_METHOD(Wrapper::Kill)
{
    if (2 == info.Length() &&
             info[0]->IsUint32() &&
             info[1]->IsFunction()) {
        uint32_t pid = info[0]->Uint32Value();
        auto callback = new Callback(info[1].As<Function>());
        Nan::AsyncQueueWorker(new KillWorker(callback, pid, 0));
    }
    else if (3 == info.Length() &&
                  info[0]->IsUint32() &&
                  info[1]->IsInt32() &&
                  info[2]->IsFunction()) {
        uint32_t pid = info[0]->Uint32Value();
        int32_t code = info[1]->Int32Value();
        auto callback = new Callback(info[2].As<Function>());
        Nan::AsyncQueueWorker(new KillWorker(callback, pid, code));
    }
    else {
        Nan::ThrowTypeError(u8"Invalid arguments");
    }
}

} // ps namespace
