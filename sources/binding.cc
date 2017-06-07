
#include "wrapper.h"

namespace ps {

NAN_MODULE_INIT(Initialize)
{
    Wrapper::Initialize(target);
}

NODE_MODULE(ps_native, Initialize);

} // ps namespace
