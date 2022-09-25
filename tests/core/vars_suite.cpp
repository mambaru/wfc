#include <fas/testing.hpp>
#include <wfc/core/vars.hpp>
#include <fstream>
UNIT(vars1, "")
{
  using namespace fas::testing;
  //std::string text="Привет $TANK{COUNT} ";
  std::string text=
    "Testing variable parsing:\n"
    "   Simple variable: ${COUNT}\n"
    "   Simple variable with default value: ${COUNT:0}\n"
    "   Variable with prefix: $ENV{ADDR}\n"
    "   Variable with prefix and default value: $ENV{ADDR:192.168.1.2}\n"
    "   Quoted variable may by all: $ENV{ADDR:\"192.168.1.2\"}\n"
    "   Not variable : $ADDR} $VAR\n"
    "   Options: ${COUNT[1,10]:1}\n"
    "   Options: ${COUNT[ from=1, \"to\"=\"10\", \"abra kadabra\"]  :  10}\n"
    "   Unknown variable: ${UNKNOWN}\n"
    "   Test: ${COUN} ${COU} ${CO} ${C }\n"
    ;


 // "Привет ${ X-.z: Y_k } $TANK{ COUNT } $ENV{ VLAUE [ opt1 , opt2 = val_2 ] : DEFAULT }    $ENV2{VLAUE2[opt21, opt22=\"val2_2-&*\"]:DEF2AULT}";
  wfc::vars::vars_list_t vars_list;
  std::string err;
  wfc::vars _vars([](const std::string& f) noexcept { return f; } );
  bool parse_status = _vars.search_vars(text, &vars_list);
  t << is_true<expect>(parse_status) << err << FAS_FL;
  t << equal<assert,size_t>(vars_list.size(), 12) << FAS_FL;
  t << flush;

  t << equal_str<expect>(vars_list[0].prefix, "") << FAS_FL;
  t << equal_str<expect>(vars_list[0].name, "COUNT") << FAS_FL;
  t << equal_str<expect>(vars_list[0].defval, "") << FAS_FL;
  t << equal<expect, size_t>(vars_list[0].options.size(), 0) << FAS_FL;

  t << equal_str<expect>(vars_list[1].prefix, "") << FAS_FL;
  t << equal_str<expect>(vars_list[1].name, "COUNT") << FAS_FL;
  t << equal_str<expect>(vars_list[1].defval, "0") << FAS_FL;
  t << equal<expect, size_t>(vars_list[1].options.size(), 0) << FAS_FL;

  t << equal_str<expect>(vars_list[2].prefix, "ENV") << FAS_FL;
  t << equal_str<expect>(vars_list[2].name, "ADDR") << FAS_FL;
  t << equal_str<expect>(vars_list[2].defval, "") << FAS_FL;
  t << equal<expect, size_t>(vars_list[0].options.size(), 0) << FAS_FL;

  t << equal_str<expect>(vars_list[3].prefix, "ENV") << FAS_FL;
  t << equal_str<expect>(vars_list[3].name, "ADDR") << FAS_FL;
  t << equal_str<expect>(vars_list[3].defval, "192.168.1.2") << FAS_FL;
  t << equal<expect, size_t>(vars_list[0].options.size(), 0) << FAS_FL;

  t << equal_str<expect>(vars_list[4].prefix, "ENV") << FAS_FL;
  t << equal_str<expect>(vars_list[4].name, "ADDR") << FAS_FL;
  t << equal_str<expect>(vars_list[4].defval, "192.168.1.2") << FAS_FL;
  t << equal<expect, size_t>(vars_list[4].options.size(), 0) << FAS_FL;

  t << equal_str<expect>(vars_list[5].prefix, "") << FAS_FL;
  t << equal_str<expect>(vars_list[5].name, "COUNT") << FAS_FL;
  t << equal_str<expect>(vars_list[5].defval, "1") << FAS_FL;
  t << equal<assert, size_t>(vars_list[5].options.size(), 2) << FAS_FL;
  t << flush;
  t << equal_str<expect>(vars_list[5].options[0].first, "") << FAS_FL;
  t << equal_str<expect>(vars_list[5].options[0].second, "1") << FAS_FL;
  t << equal_str<expect>(vars_list[5].options[1].first, "") << FAS_FL;
  t << equal_str<expect>(vars_list[5].options[1].second, "10") << FAS_FL;

  t << equal_str<expect>(vars_list[6].prefix, "") << FAS_FL;
  t << equal_str<expect>(vars_list[6].name, "COUNT") << FAS_FL;
  t << equal_str<expect>(vars_list[6].defval, "10") << FAS_FL;
  t << equal<assert, size_t>(vars_list[6].options.size(), 3) << FAS_FL;
  t << flush;
  t << equal_str<expect>(vars_list[6].options[0].first, "from") << FAS_FL;
  t << equal_str<expect>(vars_list[6].options[0].second, "1") << FAS_FL;
  t << equal_str<expect>(vars_list[6].options[1].first, "to") << FAS_FL;
  t << equal_str<expect>(vars_list[6].options[1].second, "10") << FAS_FL;
  t << equal_str<expect>(vars_list[6].options[2].first, "") << FAS_FL;
  t << equal_str<expect>(vars_list[6].options[2].second, "abra kadabra") << FAS_FL;

  wfc::vars::replace_list_t replace_list{
    {"","C","-11-"},{"","CO","-10-"},{"","COU","-9-"}, {"","COUN","-8-"},
    {"","COUNT", "-1-"}, {"","COUNT", "-2-"},
    {"ENV","ADDR","-3-"},{"ENV","ADDR","-4-"},{"ENV","ADDR","-5-"},
    {"","COUNT", "-6-"}, {"","COUNT", "-7-"}
  };
  _vars.replace_vars(replace_list, &text, nullptr);
  t << message("\n") << text;
  std::string expected_text =
    "Testing variable parsing:\n"
    "   Simple variable: -1-\n"
    "   Simple variable with default value: -2-\n"
    "   Variable with prefix: -3-\n"
    "   Variable with prefix and default value: -4-\n"
    "   Quoted variable may by all: -5-\n"
    "   Not variable : $ADDR} $VAR\n"
    "   Options: -6-\n"
    "   Options: -7-\n"
    "   Unknown variable: ${UNKNOWN}\n"
    "   Test: -8- -9- -10- -11-\n";
  t << equal_str<expect>( text, expected_text) << FAS_FL;
}

UNIT(ini1, "")
{
  using namespace fas::testing;
  std::string text=
  "; comment1\n"
  "# bad_one ; coment2\n"
  "two= ; \n"
  "three = \"42\"\n"
  " [ section1 ] \n"
  "four = four\n"
  "five  = 5.\"5\".'5'\n"
  "[section2 ]\n"
  "six =     sex\n"
  ;
  wfc::vars _vars([](const std::string& f) noexcept { return f; } );
  _vars.parse_ini(text);
  for ( auto v : _vars.ini_map() )
    t << message("ENV") << "\t"<< v.first << "=" << v.second;
}

UNIT(create_tests_files, "")
{
  using namespace fas::testing;
  t << nothing;
  std::ofstream ofs("test-ini1.ini");
  ofs << "[test]\ntest_value=\"12345\"\n";
  ofs.close();
  ofs.open("test-ini2.ini");
  ofs << "$INCLUDE{FILE[path=test-ini1.ini]}\n";
  ofs.close();
  ofs.open("test-conf1.conf");
  ofs << "{\"value\":\"$INI{test.test_value:666}\"}";
  ofs.close();
  ofs.open("test-conf2.conf");
  ofs << "{\"object\":$INCLUDE{FILE[path=\"test-conf1.conf\"]} }\n";
  ofs.close();
}

UNIT(parse_tests_files, "")
{
  using namespace fas::testing;
  wfc::vars _vars([](const std::string& f) noexcept { return f; } );
  _vars.add_ini({"test-ini2.ini"});
  bool res = _vars.parse_file("test-conf2.conf");
  t << message("Config file after parsing:\n") << _vars.result() << _vars.error_message();
  t << is_true<assert>(res) << FAS_FL;
}

BEGIN_SUITE(vars_suite, "")
  ADD_UNIT(vars1)
  ADD_UNIT(ini1)
  ADD_UNIT(create_tests_files)
  ADD_UNIT(parse_tests_files)
END_SUITE(vars_suite)

