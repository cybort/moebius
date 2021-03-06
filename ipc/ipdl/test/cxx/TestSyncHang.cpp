#include "TestSyncHang.h"
#include "base/task.h"
#include "mozilla/ipc/GoannaChildProcessHost.h"

#include "IPDLUnitTests.h"      // fail etc.

using std::vector;
using std::string;

namespace mozilla {
namespace _ipdltest {

//-----------------------------------------------------------------------------
// parent

mozilla::ipc::GoannaChildProcessHost* gSyncHangSubprocess;

TestSyncHangParent::TestSyncHangParent()
{
    MOZ_COUNT_CTOR(TestSyncHangParent);
}

TestSyncHangParent::~TestSyncHangParent()
{
    MOZ_COUNT_DTOR(TestSyncHangParent);
}

void
DeleteSyncHangSubprocess(MessageLoop* uiLoop)
{
  delete gSyncHangSubprocess;
}

void
DeferredSyncHangParentShutdown()
{
  // ping to DeleteSubprocess
  XRE_GetIOMessageLoop()->PostTask(
      NewRunnableFunction(DeleteSyncHangSubprocess, MessageLoop::current()));
}

void
TestSyncHangParent::Main()
{
  vector<string> args;
  args.push_back("fake/path");
  gSyncHangSubprocess = new mozilla::ipc::GoannaChildProcessHost(GoannaProcessType_Plugin);
  bool launched = gSyncHangSubprocess->SyncLaunch(args, 2);
  if (launched)
    fail("Calling SyncLaunch with an invalid path should return false");

  MessageLoop::current()->PostTask(NewRunnableFunction(DeferredSyncHangParentShutdown));
  Close();
}

//-----------------------------------------------------------------------------
// child

TestSyncHangChild::TestSyncHangChild()
{
    MOZ_COUNT_CTOR(TestSyncHangChild);
}

TestSyncHangChild::~TestSyncHangChild()
{
    MOZ_COUNT_DTOR(TestSyncHangChild);
}

} // namespace _ipdltest
} // namespace mozilla
