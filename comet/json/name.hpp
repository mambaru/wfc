#pragma once

#ifndef NAME
#define NAME(X) struct n_##X { const char* operator()() const{ return #X;} };
#endif

#ifndef NAME_EX
#define NAME_EX(X, Y) struct X { const char* operator()() const{ return #Y;} };
#endif


#ifndef NAME_DESC
#define NAME_DESC(X, S) struct n_##X { const char* operator()() const{ return #X;} const char* desc() const{ return S; } };
#endif
