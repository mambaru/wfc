#include <wfc/pubsub/topic_hub.hpp>

#include <vector>
#include <iostream>
#include <algorithm>

using namespace ::wfc;

int test1()
{
  topic_hub hub;
  message m;
  m.limit = 3;
  m.lifetime = 1;
  m.str("content1");
  hub.push("channel1", topic_hub::message_ptr(new message(m)) );
  hub.push("channel1", topic_hub::message_ptr(new message(m)) );
  hub.push("channel1", topic_hub::message_ptr(new message(m)) );
  
  /*hub.at("channel1")->push(m);
  hub.at("channel1")->push(m);
  */

  topic_hub::message_vector_ptr ms = hub.get_messages("channel1");
  if ( ms->size() != 3)
  {
    std::cerr << "error ms.size() = " << ms->size() << std::endl;
    return 1;
  }

  ms = hub.get_messages("channel1");
  for (const auto& m : *ms)
  {
    std::cout << m->str() << " " << m.get() << std::endl;
    if ( !m || m->str()!="content1" )
    {
      std::cerr << "error m->str() = " << m->str() << std::endl;
      return 1;
    }
  }
        

  

  /*
  std::shared_ptr<int> p1(new int(1));
  std::weak_ptr<int> p2;
  p2 = p1;
  */
  
  return 0;
}

int test2()
{
  return 0;
}



int main(int argc, char* argv[])
{
  
  return test1() + test2();
}
