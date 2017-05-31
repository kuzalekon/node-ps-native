
#include "wrapper.h"

namespace ps {

NAN_MODULE_INIT(Initialize)
{
    Nan::Export(target, "list", wrapper::List);
}

NODE_MODULE(ps, Initialize);

} // ps namespace
