#include "my_test.hpp"

#include <memory>
#include <vector>

namespace my_namespace {
void test() {
  // this causes the error:
  //
  // Error	LNK2005	"public: __cdecl std::vector<enum std::byte,class std::allocator<enum std::byte> >::~vector<enum
  // std::byte,class std::allocator<enum std::byte> >(void)"
  // (??1?$vector@W4byte@std@@V?$allocator@W4byte@std@@@2@@std@@QEAA@XZ) already defined in my_lib.lib(my_test.obj)
  // my_app	C:\Users\marek\works\sandbox\build\problem\my_app\datachannel.lib(datachannel.dll)	1 Error	LNK1169
  // one or more multiply defined symbols found	my_app
  //
  // C:\Users\marek\works\sandbox\build\problem\my_app\Debug\my_app.exe	1
  auto video_frame_ = std::make_shared<std::vector<std::byte>>(123456);
}
} // namespace my_namespace
