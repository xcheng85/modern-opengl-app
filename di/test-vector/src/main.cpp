#include "di.hpp"
#include <memory>
#include <vector>
#include <string>
#include <cassert>
#include <format>
#include <iostream>

namespace di = boost::di;
using namespace std;

class room
{
};
class speaker
{
};

struct name : std::string
{
    using std::string::string;
};

class attendees
{
public:
    explicit attendees(const std::vector<name> &names)
    {
        cout << names.size() << endl;
        assert(3u == names.size());
        cout << names[0] << endl;
        cout << names[1] << endl;
        cout << names[2] << endl;
        // assert("dallocation" == names[0]);
        // assert("performancequery" == names[1]);
        // assert("hostqueryreset" == names[2]);
    }
};

class cppcon_talk
{
public:
    cppcon_talk(room &&, std::unique_ptr<speaker>, attendees &) {}
};

struct i1 : std::string
{
    virtual ~i1() noexcept = default;
    virtual void dummy1() = 0;
};
struct impl : i1
{
    void dummy1() override {}
};
struct impl1 : i1
{
    void dummy1() override {}
};
//->

// int main() {
//   // clang-format off
//   auto injector = di::make_injector(
//     di::bind<i1*[]>().to<impl, impl1>()
//   );
//   // clang-format on

//   auto v = injector.create<std::vector<std::unique_ptr<i1>>>();
//   assert(2 == v.size());
//   assert(dynamic_cast<impl*>(v[0].get()));
//   assert(dynamic_cast<impl1*>(v[1].get()));
// }

int main()
{
    // const auto injector = di::make_injector(
    //     di::bind<name[]>.to({name{"ddddddallocation"},
    //                          name{"ancequery"},
    //                          name{"reset"}}));
    // injector.create<cppcon_talk>();

    // clang-format off
  std::initializer_list<std::string> il ={"VK_KHR_dedicated_allocation", "ancequery", "fffffffff"};
  auto injector = di::make_injector(
    di::bind<std::string[]>().to(il) // or di::bind<int*[]>.to(il)
  );
    // clang-format on

    auto names = injector.create<std::vector<std::string>>();
    cout << names[0] << endl;
    cout << names[1] << endl;
    cout << names[2] << endl;
}
