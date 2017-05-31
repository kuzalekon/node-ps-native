
#include "wrapper.h"

#define REQARG_OBJECT(i, var)                                       \
    if (info.Length() <= (i) || !info[i]->IsFunction()) {                       \
        return Nan::ThrowTypeError("Argument " #i " must be a object");       \
    }                                                                           \
    v8::Local<v8::Object> var = v8::Local<v8::Object>::Cast(info[i]);

#define REQARG_FUNCTION(i, var)                                       \
    if (info.Length() <= (i) || !info[i]->IsFunction()) {                       \
        return Nan::ThrowTypeError("Argument " #i " must be a function");       \
    }                                                                           \
    v8::Local<v8::Function> var = v8::Local<v8::Function>::Cast(info[i]);

#define REQARG_UINT(i, var)                                         \
    if (info.Length() <= (i) || !info[i]->IsUint32()) {                         \
        return Nan::ThrowTypeError("Argument " #i " must be a uint");           \
    }                                                                           \
    uint32_t var = Nan::To<uint32_t>(info[i]).FromJust();

#define REQARG_INT(i, var)                                          \
    if (info.Length() <= (i) || !info[i]->IsInt32()) {                         \
        return Nan::ThrowTypeError("Argument " #i " must be a int");            \
    }                                                                           \
    int32_t var = Nan::To<int32_t>(info[i]).FromJust();

#define OPTARG_UINT(i, var, val)                               \
    uint32_t var = (val);                                                   \
    if (info.Length() >= i && info[i]->IsUint32()) {                            \
        var = Nan::To<uint32_t>(info[i]).FromJust();                            \
    }                                                                           \
    else {                                                                      \
        return Nan::ThrowTypeError("Argument " #i " must be a uint");           \
    }

#define OPTARG_FUNCTION(i, var)                               \
    v8::Local<v8::Function> var;                                                   \
    if (info.Length() >= i && info[i]->IsFunction()) {                            \
        var = v8::Local<v8::Function>::Cast(info[i]);                        \
    }                                                                           \
    else {                                                                      \
        return Nan::ThrowTypeError("Argument " #i " must be a function");           \
    }

namespace ps {
namespace wrapper {

using std::runtime_error;

using native::Process;
using native::ProcessList;

using v8::Local;
using v8::Value;
using v8::Object;
using v8::Uint32;
using v8::String;
using v8::Array;
using v8::Function;

using Nan::HandleScope;
using Nan::AsyncWorker;
using Nan::Callback;

namespace { // anonymous

Local<Array> PackProcesses(const ProcessList& processes)
{
    auto jsProcesses = Nan::New<Array>(static_cast<uint32_t>(processes.size()));
    uint32_t index = 0;
    for (auto it = processes.begin(); it != processes.end(); ++it) {
        auto jsProcess = Nan::New<Object>();
        Nan::Set(jsProcess, Nan::New<String>("pid").ToLocalChecked(),
                 Nan::New<Uint32>(it->pid));
        Nan::Set(jsProcess, Nan::New<String>("parent").ToLocalChecked(),
                 Nan::New<Uint32>(it->parent));
        Nan::Set(jsProcess, Nan::New<String>("priority").ToLocalChecked(),
                 Nan::New<Uint32>(it->priority));
        Nan::Set(jsProcess, Nan::New<String>("threads").ToLocalChecked(),
                 Nan::New<Uint32>(it->threads));
        Nan::Set(jsProcess, Nan::New<String>("name").ToLocalChecked(),
                 Nan::New<String>(&it->name[0]).ToLocalChecked());

        Nan::Set(jsProcesses, index, jsProcess);
        index++;
    }

    return jsProcesses;
}

void UnpackKillOptions(const Local<Object>& options, uint32_t& pid, int32_t& code, uint32_t& timeout)
{
    pid = 0;
    code = 0;
    timeout = 1000;

    auto pidProp = Nan::New<String>("pid").ToLocalChecked();
    auto codeProp = Nan::New<String>("code").ToLocalChecked();
    auto timeoutProp = Nan::New<String>("timeout").ToLocalChecked();

    if (Nan::Has(options, pidProp).FromJust()) {
        pid = Nan::Get(options, pidProp).ToLocalChecked()->Uint32Value();
    }

    if (Nan::Has(options, codeProp).FromJust()) {
        pid = Nan::Get(options, codeProp).ToLocalChecked()->Int32Value();
    }

    if (Nan::Has(options, timeoutProp).FromJust()) {
        pid = Nan::Get(options, timeoutProp).ToLocalChecked()->Uint32Value();
    }
}

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

    const int argc = 2;
    Local<Value> argv[] = { Nan::Null(), PackProcesses(processes_) };
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
        native::Kill(pid_, code_, timeout_);
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

    // Execute in sync mode
    if (0 == info.Length()) {
        try {
            ProcessList processes = native::List();
            info.GetReturnValue().Set(PackProcesses(processes));
        }
        catch (const runtime_error& error) {
            return Nan::ThrowError(error.what());
        }
    }
    // Execute in async mode
    else {
        REQARG_FUNCTION(0, callback);

        Nan::AsyncQueueWorker(new ListWorker(new Callback(callback)));
    }
}

NAN_METHOD(Kill)
{
    HandleScope scope;

    uint32_t pid = 0;
    int32_t code = 0;
    uint32_t timeout = 1000;

    REQARG_OBJECT(0, options);
    OPTARG_FUNCTION(1, callback);
    
    UnpackKillOptions(options, pid, code, timeout);

    // Execute in sync mode
    if (1 == info.Length()) {
        try {
            native::Kill(pid, code, timeout);
        }
        catch (const runtime_error& error) {
            return Nan::ThrowError(error.what());
        }
    }
    // Execute in async mode
    else {
        Nan::AsyncQueueWorker(new ListWorker(new Callback(callback)));
    }
}

} // wrapper namespace
} // ps namespace
