#include <iostream>

#include "SongFilter.h"

void ComplexFilterTest(){
  YTMClient::SongFilter filt1;
  filt1.ParseFrom("\\t<Test>");
  YTMClient::SongFilter filt2;
  filt2.ParseFrom("\\a<Cake>");
  YTMClient::SongFilter filt3;
  filt3.ParseFrom("\\l<Ekac>");
  YTMClient::SongFilter filt4;
  filt4.ParseFrom("\\a<Cake> \\t<Test>");
  YTMClient::SongFilter filt5;
  filt5.ParseFrom("\\l<Ekac> \\t<Test>");
  YTMClient::SongFilter filt6;
  filt6.ParseFrom("\\l<Ekac> \\a<Cake>");
  YTMClient::SongFilter filt7;
  filt7.ParseFrom("\\l<Ekac> \\t<Test> \\a<Cake>");


  YTMClient::song_entry t1 {"Test", "asdf", "fdsa", 0};
  YTMClient::song_entry t2 {"qwer", "Cake", "fdsa", 0};
  YTMClient::song_entry t3 {"qwer", "asdf", "Ekac", 0};
  YTMClient::song_entry t4 {"Test", "Cake", "fdsa", 0};
  YTMClient::song_entry t5 {"Test", "asdf", "Ekac", 0};
  YTMClient::song_entry t6 {"qwer", "Cake", "Ekac", 0};
  YTMClient::song_entry t7 {"Test", "Cake", "Ekac", 0};

  std::cout << filt1.Test(t1)
            << filt1.Test(t2)
            << filt1.Test(t3)
            << filt1.Test(t4)
            << filt1.Test(t5)
            << filt1.Test(t6)
            << filt1.Test(t7) << std::endl << "1001101" << std::endl;;
  std::cout << filt2.Test(t1)
            << filt2.Test(t2)
            << filt2.Test(t3)
            << filt2.Test(t4)
            << filt2.Test(t5)
            << filt2.Test(t6)
            << filt2.Test(t7) << std::endl << "0101011" << std::endl;;
  std::cout << filt3.Test(t1)
            << filt3.Test(t2)
            << filt3.Test(t3)
            << filt3.Test(t4)
            << filt3.Test(t5)
            << filt3.Test(t6)
            << filt3.Test(t7) << std::endl << "0010111" << std::endl;;
  std::cout << filt4.Test(t1)
            << filt4.Test(t2)
            << filt4.Test(t3)
            << filt4.Test(t4)
            << filt4.Test(t5)
            << filt4.Test(t6)
            << filt4.Test(t7) << std::endl << "0001001" << std::endl;;
  std::cout << filt5.Test(t1)
            << filt5.Test(t2)
            << filt5.Test(t3)
            << filt5.Test(t4)
            << filt5.Test(t5)
            << filt5.Test(t6)
            << filt5.Test(t7) << std::endl << "0000101" << std::endl;;
  std::cout << filt6.Test(t1)
            << filt6.Test(t2)
            << filt6.Test(t3)
            << filt6.Test(t4)
            << filt6.Test(t5)
            << filt6.Test(t6)
            << filt6.Test(t7) << std::endl << "0000011" << std::endl;;
  std::cout << filt7.Test(t1)
            << filt7.Test(t2)
            << filt7.Test(t3)
            << filt7.Test(t4)
            << filt7.Test(t5)
            << filt7.Test(t6)
            << filt7.Test(t7) << std::endl << "0000001" << std::endl;;
}
void SimpleFilterTest(){
  YTMClient::SongFilter filt1;
  filt1.ParseFrom("Test");

  YTMClient::song_entry t1 {"Test", "asdf", "fdsa", 0};
  YTMClient::song_entry t2 {"qwer", "Test", "fdsa", 0};
  YTMClient::song_entry t3 {"qwer", "asdf", "Test", 0};
  YTMClient::song_entry t4 {"qwer", "Test", "Test", 0};
  YTMClient::song_entry t5 {"Test", "Test", "fdsa", 0};
  YTMClient::song_entry t6 {"Test", "asdf", "Test", 0};
  YTMClient::song_entry t7 {"Test", "Test", "Test", 0};
  YTMClient::song_entry t8 {"aasdfasdfasTestsadfsadfsadf", "asdf", "fdsa", 0};
  YTMClient::song_entry t9 {"qwer", "the quick brown fox Test jumped over the lazy dog.", "fdsa", 0};
  YTMClient::song_entry t10 {"qwer", "asdf", "sadfasf fsafa Test asdfaag dsagasd", 0};
  YTMClient::song_entry t11 {"qwer", "qqqq qqqq qqqTestasfaf", "zzzTestzzz", 0};
  YTMClient::song_entry t12 {"Test", "238`23942139rjfiqoj'Testasdfflewijfwei", "fdsa", 0};
  YTMClient::song_entry t13 {"   Test  ", "asdf", "qqqqTestasdfad", 0};
  YTMClient::song_entry t14 {"zTest", "Testf", "qTest", 0};

  YTMClient::song_entry f1 {"qwer", "asdf", "fdsa", 0};
  YTMClient::song_entry f2 {"test", "asdf", "fdsa", 0};
  YTMClient::song_entry f3 {"aaatestaaa", "asdf", "fdsa", 0};

  std::cout << filt1.Test(t1)
            << filt1.Test(t2)
            << filt1.Test(t3)
            << filt1.Test(t4)
            << filt1.Test(t5)
            << filt1.Test(t6)
            << filt1.Test(t7)
            << filt1.Test(t8)
            << filt1.Test(t9);
  filt1.ParseFrom("Test");
  std::cout << filt1.Test(t10)
            << filt1.Test(t11)
            << filt1.Test(t12)
            << filt1.Test(t13)
            << filt1.Test(t14) << std::endl;
  std::cout << filt1.Test(f1)
            << filt1.Test(f2)
            << filt1.Test(f3) << std::endl;
}

int main() {
  SimpleFilterTest();
  ComplexFilterTest();
}

