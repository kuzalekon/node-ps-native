
#include "wrapper.h"

#define REQUIRE_ARGUMENT_FUNCTION(i, var)                                      \
    if (info.Length() <= (i) || !info[i]->IsFunction()) {                      \
        return Nan::ThrowTypeError("Argument " #i " must be a function");      \
    }                                                                          \
    Local<Function> var = Local<Function>::Cast(info[i]);

namespace ps {
namespace wrapper {

using std::runtime_error;

using native::Process;
using native::ProcessList;

using v8::Local;
using v8::Value;
using v8::Object;
using v8::Number;
using v8::String;
using v8::Array;
using v8::Function;

using Nan::HandleScope;
using Nan::AsyncWorker;
using Nan::Callback;

namespace { // anonymous

Local<Array> PackProcesses(const ProcessList& processes)
{
    auto jsProcesses = Nan::New<Array>(processes.size());
    size_t index = 0;
    for (auto it = processes.begin(); it != processes.end(); ++it) {
        auto jsProcess = Nan::New<Object>();
        Nan::Set(jsProcess, Nan::New<String>("pid").ToLocalChecked(),
                 Nan::New<Number>(it->pid));
        Nan::Set(jsProcess, Nan::New<String>("ppid").ToLocalChecked(),
                 Nan::New<Number>(it->ppid));
        Nan::Set(jsProcess, Nan::New<String>("exe").ToLocalChecked(),
                 Nan::New<String>(it->exe.data()).ToLocalChecked());
        Nan::Set(jsProcess, Nan::New<String>("priority").ToLocalChecked(),
                 Nan::New<Number>(it->priority));
        Nan::Set(jsProcess, Nan::New<String>("threads").ToLocalChecked(),
                 Nan::New<Number>(it->threads));

        Nan::Set(jsProcesses, index, jsProcess);
        index++;
    }

    return jsProcesses;
}

} // anonymous namespace

void EnumWorker::Execute()
{
    try {
        processes_ = native::List();
    }
    catch (const runtime_error& error) {
        SetErrorMessage(error.what());
    }
}

void EnumWorker::HandleOKCallback()
{
    HandleScope scope;

    const int argc = 2;
    Local<Value> argv[] = { Nan::Null(), PackProcesses(processes_) };
    callback->Call(argc, argv);
}

void EnumWorker::HandleErrorCallback()
{
    HandleScope scope;

    const int argc = 2;
    Local<Value> argv[] = { Nan::Error(ErrorMessage()), Nan::Null() };
    callback->Call(argc, argv);
}

NAN_METHOD(List)
{
    Nan::HandleScope scope;

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
        REQUIRE_ARGUMENT_FUNCTION(0, callback);

        Nan::AsyncQueueWorker(new EnumWorker(new Callback(callback)));
    }
}

} // wrapper namespace
} // ps namespace
