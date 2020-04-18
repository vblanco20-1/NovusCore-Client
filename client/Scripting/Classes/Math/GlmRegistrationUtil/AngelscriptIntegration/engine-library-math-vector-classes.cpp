#include "../GlmRegistrationUtil.h"
#include "include-glm.h"
#include <angelscript.h>

#pragma warning(push)
#pragma warning(disable: 4244 4146)


namespace AngelScriptIntegration {

void AngelScriptCheck(int returnCode)
{
    assert(returnCode >= 0);
}

template<typename T_target, typename T_source>
T_target cast_vector1(const T_source* source)
{
  return T_target(source->x);
}

template<typename T_target, typename T_source>
T_target cast_vector2(const T_source* source)
{
  return T_target(source->x, source->y);
}

template<typename T_target, typename T_source>
T_target cast_vector3(const T_source* source)
{
  return T_target(source->x, source->y, source->z);
}

template<typename T_target, typename T_source>
T_target cast_vector4(const T_source* source)
{
  return T_target(source->x, source->y, source->z, source->w);
}

#define REGISTER_CAST_(source_vec, target_vec, vecLength) \
  r = as_engine->RegisterObjectMethod(#source_vec#vecLength, #target_vec#vecLength" opConv()", asFUNCTIONPR(cast_vector##vecLength, (const source_vec##vecLength*), target_vec##vecLength), asCALL_CDECL_OBJFIRST);AngelScriptCheck(r); \
 
#define REGISTER_CAST(source_vec, target_vec) \
  REGISTER_CAST_(source_vec, target_vec, 2) \
  REGISTER_CAST_(source_vec, target_vec, 3) \
  REGISTER_CAST_(source_vec, target_vec, 4)

template<typename T_vec>
inline typename T_vec::value_type getVecElement(T_vec* v, int i)
{
  if(i>=0 && i<v->length())
  {
    return (*v)[i];
  }
    else
  {
    return (*v)[glm::clamp<int>(i, 0, v->length()-1)];
  }
}

#define REGISTER_BASIC_OPERATORS_(vec, type) \
  r = as_engine->RegisterObjectMethod(#vec, #type" opIndex(int) const", asFUNCTION(getVecElement<vec>), asCALL_CDECL_OBJFIRST);AngelScriptCheck(r); \
 
#define REGISTER_BASIC_OPERATORS(prefix, type) \
  REGISTER_BASIC_OPERATORS_(prefix##vec##2, type) \
  REGISTER_BASIC_OPERATORS_(prefix##vec##3, type) \
  REGISTER_BASIC_OPERATORS_(prefix##vec##4, type)

template<typename R, typename A, typename B>
R opAdd(const A& a, const B& b)
{
  return a+b;
}

template<typename R, typename A, typename B>
R opSub(const A& a, const B& b)
{
  return a-b;
}

template<typename R, typename A, typename B>
R opSub_r(const A& a, const B& b)
{
  return b-a;
}

template<typename R, typename A, typename B>
R opMul(const A& a, const B& b)
{
  return a*b;
}

template<typename A, typename B>
A opAddAssign(A& a, const B& b)
{
  return a+=b;
}

template<typename A, typename B>
A opSubAssign(A& a, const B& b)
{
  return a-=b;
}

template<typename A, typename B>
A opMulAssign(A& a, const B& b)
{
  return a*=b;
}

template<typename A>
A opNeg(const A& a)
{
  return -a;
}

template<typename A>
A opPreInc(A& a)
{
  return ++a;
}

template<typename A>
A opPreDec(A& a)
{
  return --a;
}

template<typename A>
A opPostInc(A& a)
{
  return a++;
}

template<typename A>
A opPostDec(A& a)
{
  return a--;
}

template<typename A, typename B>
bool opEquals(const A& a, const B& b)
{
  return a == A(b);
}

#define REGISTER_NUMERIC_OPERATORS_(vec, type) \
  r = as_engine->RegisterObjectMethod(#vec, #vec" opAdd("#vec")", asFUNCTION((opAdd<vec, vec, vec>)), asCALL_CDECL_OBJFIRST);AngelScriptCheck(r); \
  r = as_engine->RegisterObjectMethod(#vec, #vec" opAdd("#type")", asFUNCTION((opAdd<vec, vec, type>)), asCALL_CDECL_OBJFIRST);AngelScriptCheck(r); \
  r = as_engine->RegisterObjectMethod(#vec, #vec" opAdd_r("#type")", asFUNCTION((opAdd<vec, vec, type>)), asCALL_CDECL_OBJFIRST);AngelScriptCheck(r); \
  r = as_engine->RegisterObjectMethod(#vec, #vec" opSub("#vec")", asFUNCTION((opSub<vec, vec, vec>)), asCALL_CDECL_OBJFIRST);AngelScriptCheck(r); \
  r = as_engine->RegisterObjectMethod(#vec, #vec" opSub("#type")", asFUNCTION((opSub<vec, vec, type>)), asCALL_CDECL_OBJFIRST);AngelScriptCheck(r); \
  r = as_engine->RegisterObjectMethod(#vec, #vec" opSub_r("#type")", asFUNCTION((opSub_r<vec, vec, type>)), asCALL_CDECL_OBJFIRST);AngelScriptCheck(r); \
  r = as_engine->RegisterObjectMethod(#vec, #vec" opMul("#vec")", asFUNCTION((opMul<vec, vec, vec>)), asCALL_CDECL_OBJFIRST);AngelScriptCheck(r); \
  r = as_engine->RegisterObjectMethod(#vec, #vec" opMul("#type")", asFUNCTION((opMul<vec, vec, type>)), asCALL_CDECL_OBJFIRST);AngelScriptCheck(r); \
  r = as_engine->RegisterObjectMethod(#vec, #vec" opMul_r("#type")", asFUNCTION((opMul<vec, vec, type>)), asCALL_CDECL_OBJFIRST);AngelScriptCheck(r); \
  r = as_engine->RegisterObjectMethod(#vec, #vec" opAddAssign("#vec")", asFUNCTION((opAddAssign<vec, vec>)), asCALL_CDECL_OBJFIRST);AngelScriptCheck(r); \
  r = as_engine->RegisterObjectMethod(#vec, #vec" opAddAssign("#type")", asFUNCTION((opAddAssign<vec, type>)), asCALL_CDECL_OBJFIRST);AngelScriptCheck(r); \
  r = as_engine->RegisterObjectMethod(#vec, #vec" opSubAssign("#vec")", asFUNCTION((opSubAssign<vec, vec>)), asCALL_CDECL_OBJFIRST);AngelScriptCheck(r); \
  r = as_engine->RegisterObjectMethod(#vec, #vec" opSubAssign("#type")", asFUNCTION((opSubAssign<vec, type>)), asCALL_CDECL_OBJFIRST);AngelScriptCheck(r); \
  r = as_engine->RegisterObjectMethod(#vec, #vec" opMulAssign("#vec")", asFUNCTION((opMulAssign<vec, vec>)), asCALL_CDECL_OBJFIRST);AngelScriptCheck(r); \
  r = as_engine->RegisterObjectMethod(#vec, #vec" opMulAssign("#type")", asFUNCTION((opMulAssign<vec, type>)), asCALL_CDECL_OBJFIRST);AngelScriptCheck(r); \
  r = as_engine->RegisterObjectMethod(#vec, #vec" opNeg()", asFUNCTION((opNeg<vec>)), asCALL_CDECL_OBJFIRST);AngelScriptCheck(r); \
  r = as_engine->RegisterObjectMethod(#vec, #vec" opPreInc()", asFUNCTION((opPreInc<vec>)), asCALL_CDECL_OBJFIRST);AngelScriptCheck(r); \
  r = as_engine->RegisterObjectMethod(#vec, #vec" opPreDec()", asFUNCTION((opPreDec<vec>)), asCALL_CDECL_OBJFIRST);AngelScriptCheck(r); \
  r = as_engine->RegisterObjectMethod(#vec, #vec" opPostInc()", asFUNCTION((opPostInc<vec>)), asCALL_CDECL_OBJFIRST);AngelScriptCheck(r); \
  r = as_engine->RegisterObjectMethod(#vec, #vec" opPostDec()", asFUNCTION((opPostDec<vec>)), asCALL_CDECL_OBJFIRST);AngelScriptCheck(r); \
  r = as_engine->RegisterObjectMethod(#vec, "bool opEquals("#vec")", asFUNCTION((opEquals<vec, vec>)), asCALL_CDECL_OBJFIRST);AngelScriptCheck(r); \
  r = as_engine->RegisterObjectMethod(#vec, "bool opEquals("#type")", asFUNCTION((opEquals<vec, type>)), asCALL_CDECL_OBJFIRST);AngelScriptCheck(r); \
 

#define REGISTER_NUMERIC_OPERATORS(prefix, type) \
  REGISTER_BASIC_OPERATORS(prefix, type) \
  REGISTER_NUMERIC_OPERATORS_(prefix##vec##2, type) \
  REGISTER_NUMERIC_OPERATORS_(prefix##vec##3, type) \
  REGISTER_NUMERIC_OPERATORS_(prefix##vec##4, type)


template<typename R, typename A, typename B>
R opDiv_real(const A* a, B b)
{
  return *a/b;
}

template<typename R, typename A, typename B>
R opDiv_r_real(const A* a, B b)
{
  return b/ *a;
}

template<typename A, typename B>
A opDivAssign_real(A* a, B b)
{
  return (*a)/=b;
}

#define REGISTER_REAL_OPERATORS_(vec, type) \
  r = as_engine->RegisterObjectMethod(#vec, #vec" opDiv("#vec")", asFUNCTION((opDiv_real<vec, vec, vec>)), asCALL_CDECL_OBJFIRST);AngelScriptCheck(r); \
  r = as_engine->RegisterObjectMethod(#vec, #vec" opDiv("#type")", asFUNCTION((opDiv_real<vec, vec, type>)), asCALL_CDECL_OBJFIRST);AngelScriptCheck(r); \
  r = as_engine->RegisterObjectMethod(#vec, #vec" opDiv_r("#type")", asFUNCTION((opDiv_r_real<vec, vec, type>)), asCALL_CDECL_OBJFIRST);AngelScriptCheck(r); \
  r = as_engine->RegisterObjectMethod(#vec, #vec" opDivAssign("#vec")", asFUNCTION((opDivAssign_real<vec, vec>)), asCALL_CDECL_OBJFIRST);AngelScriptCheck(r); \
  r = as_engine->RegisterObjectMethod(#vec, #vec" opDivAssign("#type")", asFUNCTION((opDivAssign_real<vec, type>)), asCALL_CDECL_OBJFIRST);AngelScriptCheck(r); \
 

#define REGISTER_REAL_OPERATORS(prefix, type) \
  REGISTER_NUMERIC_OPERATORS(prefix, type) \
  REGISTER_REAL_OPERATORS_(prefix##vec##2, type) \
  REGISTER_REAL_OPERATORS_(prefix##vec##3, type) \
  REGISTER_REAL_OPERATORS_(prefix##vec##4, type)


template<typename R, typename A, typename B>
R opMod_vec_vec(const A* a, B b)
{
  for(int i=0; i<b.length(); ++i)
  {
    if(b[i] == 0)
    {
      assert(false); // Division by zero
      b[i] = 1;
    }
  }

  return *a%b;
}

template<typename R, typename A, typename B>
R opMod_vec_type(const A* a, B b)
{
  if(b == 0)
  {
    assert(false); // Division by zero
    b = 1;
  }

  return *a%b;
}

template<typename R, typename A, typename B>
R opMod_r_vec_type(const A* a_, B b)
{
  A a = *a_;

  for(int i=0; i<a.length(); ++i)
  {
    if(a[i] == 0)
    {
      assert(false); // Division by zero
      a[i] = 1;
    }
  }

  return b%a;
}


template<typename R, typename A, typename B>
R opDiv_int_vec_vec(const A* a_, B b)
{
  const A& a = *a_;

  for(int i=0; i<b.length(); ++i)
  {
    if(b[i] == 0)
    {
      assert(false); // Division by zero
      b[i] = std::numeric_limits<typename A::value_type>::max();
    }
  }

  return a/b;
}

template<typename R, typename A, typename B>
R opDiv_int_vec_type(const A* a_, B b)
{
  const A& a = *a_;

  if(b == 0)
  {
    assert(false); // Division by zero
    b = std::numeric_limits<typename A::value_type>::max();
  }

  return a/b;
}

template<typename R, typename A, typename B>
R opDiv_r_int_vec_type(const A* a_, B b)
{
  A a = *a_;

  for(int i=0; i<a.length(); ++i)
  {
    if(a[i] == 0)
    {
      assert(false); // Division by zero
      a[i] = std::numeric_limits<typename A::value_type>::max();
    }
  }

  return b/a;
}


template<typename R, typename A, typename B>
R opDivAssign_int_vec_vec(A* a_, B b)
{
  A& a = *a_;

  for(int i=0; i<b.length(); ++i)
  {
    if(b[i] == 0)
    {
      assert(false); // Division by zero
      b[i] = std::numeric_limits<typename A::value_type>::max();
    }
  }

  return a/=b;
}

template<typename R, typename A, typename B>
R opDivAssign_int_vec_type(A* a_, B b)
{
  A& a = *a_;

  if(b == 0)
  {
    assert(false); // Division by zero
    b = std::numeric_limits<typename A::value_type>::max();
  }

  return a/=b;
}


template<typename R, typename A, typename B>
R opModAssign_vec_vec(A* a_, B b)
{
  A& a = *a_;

  for(int i=0; i<b.length(); ++i)
  {
    if(b[i] == 0)
    {
      assert(false); // Division by zero
      b[i] = 1;
    }
  }

  return a%=b;
}

template<typename R, typename A, typename B>
R opModAssign_vec_type(A* a_, B b)
{
  A& a = *a_;

  if(b == 0)
  {
    assert(false); // Division by zero
    b = 1;
  }

  return a%=b;
}

template<typename A>
A opCom(const A* a_)
{
  const A& a = *a_;

  return ~a;
}

template<typename A, typename B>
A opShlAssign_scalar(A* a_, B b)
{
  A& a = *a_;

  for(int i=0; i<a.length(); ++i)
    a[i] <<= b;
  return a;
}

template<typename A, typename B>
A opShrAssign_scalar(A* a_, B b)
{
  A& a = *a_;

  for(int i=0; i<a.length(); ++i)
    a[i] >>= b;
  return a;
}

template<typename A, typename B>
A opShlAssign_vec(A* a_, B b)
{
  A& a = *a_;

  for(int i=0; i<a.length(); ++i)
    a[i] <<= b[i];
  return a;
}

template<typename A, typename B>
A opShrAssign_vec(A* a_, B b)
{
  A& a = *a_;

  for(int i=0; i<a.length(); ++i)
    a[i] >>= b[i];
  return a;
}

template<typename A, typename B>
A opShl_scalar(const A* a_, B b)
{
  A a = *a_;

  opShlAssign_scalar(&a, b);
  return a;
}

template<typename A, typename B>
A opShr_scalar(const A* a_, B b)
{
  A a = *a_;

  opShrAssign_scalar(&a, b);
  return a;
}

template<typename A, typename B>
A opShl_vec(const A* a_, B b)
{
  A a = *a_;

  opShlAssign_vec(&a, b);
  return a;
}

template<typename A, typename B>
A opShr_vec(const A* a_, B b)
{
  A a = *a_;

  opShrAssign_vec(&a, b);
  return a;
}

template<typename A, typename B>
A opAndAssign_scalar(A* a_, B b)
{
  A& a = *a_;

  for(int i=0; i<a.length(); ++i)
    a[i] &= b;
  return a;
}

template<typename A, typename B>
A opAndAssign_vec(A* a_, B b)
{
  A& a = *a_;

  for(int i=0; i<a.length(); ++i)
    a[i] &= b[i];
  return a;
}

template<typename A, typename B>
A opAnd_scalar(const A* a_, B b)
{
  A a = *a_;

  opAndAssign_scalar(&a, b);
  return a;
}

template<typename A, typename B>
A opAnd_vec(const A* a_, B b)
{
  A a = *a_;

  opAndAssign_vec(&a, b);
  return a;
}

template<typename A, typename B>
A opOrAssign_scalar(A* a_, B b)
{
  A& a = *a_;

  for(int i=0; i<a.length(); ++i)
    a[i] |= b;
  return a;
}

template<typename A, typename B>
A opOrAssign_vec(A* a_, B b)
{
  A& a = *a_;

  for(int i=0; i<a.length(); ++i)
    a[i] |= b[i];
  return a;
}

template<typename A, typename B>
A opOr_scalar(const A* a_, B b)
{
  A a = *a_;

  opOrAssign_scalar(&a, b);
  return a;
}

template<typename A, typename B>
A opOr_vec(const A* a_, B b)
{
  A a = *a_;

  opOrAssign_vec(&a, b);
  return a;
}

template<typename A, typename B>
A opXorAssign_scalar(A* a_, B b)
{
  A& a = *a_;

  for(int i=0; i<a.length(); ++i)
    a[i] ^= b;
  return a;
}

template<typename A, typename B>
A opXorAssign_vec(A* a_, B b)
{
  A& a = *a_;

  for(int i=0; i<a.length(); ++i)
    a[i] ^= b[i];
  return a;
}

template<typename A, typename B>
A opXor_scalar(const A* a_, B b)
{
  A a = *a_;

  opXorAssign_scalar(&a, b);
  return a;
}

template<typename A, typename B>
A opXor_vec(const A* a_, B b)
{
  A a = *a_;

  opXorAssign_vec(&a, b);
  return a;
}

#define REGISTER_INTEGER_OPERATORS_(vec, type) \
  r = as_engine->RegisterObjectMethod(#vec, #vec" opMod("#vec")", asFUNCTION((opMod_vec_vec<vec, vec, vec>)), asCALL_CDECL_OBJFIRST);AngelScriptCheck(r); \
  r = as_engine->RegisterObjectMethod(#vec, #vec" opMod("#type")", asFUNCTION((opMod_vec_type<vec, vec, type>)), asCALL_CDECL_OBJFIRST);AngelScriptCheck(r); \
  r = as_engine->RegisterObjectMethod(#vec, #vec" opMod_r("#type")", asFUNCTION((opMod_r_vec_type<vec, vec, type>)), asCALL_CDECL_OBJFIRST);AngelScriptCheck(r); \
  r = as_engine->RegisterObjectMethod(#vec, #vec" opDiv("#vec")", asFUNCTION((opDiv_int_vec_vec<vec, vec, vec>)), asCALL_CDECL_OBJFIRST);AngelScriptCheck(r); \
  r = as_engine->RegisterObjectMethod(#vec, #vec" opDiv("#type")", asFUNCTION((opDiv_int_vec_type<vec, vec, type>)), asCALL_CDECL_OBJFIRST);AngelScriptCheck(r); \
  r = as_engine->RegisterObjectMethod(#vec, #vec" opDiv_r("#type")", asFUNCTION((opDiv_r_int_vec_type<vec, vec, type>)), asCALL_CDECL_OBJFIRST);AngelScriptCheck(r); \
  r = as_engine->RegisterObjectMethod(#vec, #vec" opDivAssign("#vec")", asFUNCTION((opDivAssign_int_vec_vec<vec, vec, vec>)), asCALL_CDECL_OBJFIRST);AngelScriptCheck(r); \
  r = as_engine->RegisterObjectMethod(#vec, #vec" opDivAssign("#type")", asFUNCTION((opDivAssign_int_vec_type<vec, vec, type>)), asCALL_CDECL_OBJFIRST);AngelScriptCheck(r); \
  r = as_engine->RegisterObjectMethod(#vec, #vec" opModAssign("#vec")", asFUNCTION((opModAssign_vec_vec<vec, vec, vec>)), asCALL_CDECL_OBJFIRST);AngelScriptCheck(r); \
  r = as_engine->RegisterObjectMethod(#vec, #vec" opModAssign("#type")", asFUNCTION((opModAssign_vec_type<vec, vec, type>)), asCALL_CDECL_OBJFIRST);AngelScriptCheck(r); \
  r = as_engine->RegisterObjectMethod(#vec, #vec" opCom()", asFUNCTION((opCom<vec>)), asCALL_CDECL_OBJFIRST);AngelScriptCheck(r); \
  r = as_engine->RegisterObjectMethod(#vec, #vec" opShl(int)", asFUNCTION((opShl_scalar<vec, int>)), asCALL_CDECL_OBJFIRST);AngelScriptCheck(r); \
  r = as_engine->RegisterObjectMethod(#vec, #vec" opShr(int)", asFUNCTION((opShr_scalar<vec, int>)), asCALL_CDECL_OBJFIRST);AngelScriptCheck(r); \
  r = as_engine->RegisterObjectMethod(#vec, #vec" opShl(uint)", asFUNCTION((opShl_scalar<vec, uint>)), asCALL_CDECL_OBJFIRST);AngelScriptCheck(r); \
  r = as_engine->RegisterObjectMethod(#vec, #vec" opShr(uint)", asFUNCTION((opShr_scalar<vec, uint>)), asCALL_CDECL_OBJFIRST);AngelScriptCheck(r); \
  r = as_engine->RegisterObjectMethod(#vec, #vec" opShl("#vec")", asFUNCTION((opShl_vec<vec, vec>)), asCALL_CDECL_OBJFIRST);AngelScriptCheck(r); \
  r = as_engine->RegisterObjectMethod(#vec, #vec" opShr("#vec")", asFUNCTION((opShr_vec<vec, vec>)), asCALL_CDECL_OBJFIRST);AngelScriptCheck(r); \
  r = as_engine->RegisterObjectMethod(#vec, #vec" opShlAssign(int)", asFUNCTION((opShlAssign_scalar<vec, int>)), asCALL_CDECL_OBJFIRST);AngelScriptCheck(r); \
  r = as_engine->RegisterObjectMethod(#vec, #vec" opShrAssign(int)", asFUNCTION((opShrAssign_scalar<vec, int>)), asCALL_CDECL_OBJFIRST);AngelScriptCheck(r); \
  r = as_engine->RegisterObjectMethod(#vec, #vec" opShlAssign(uint)", asFUNCTION((opShlAssign_scalar<vec, uint>)), asCALL_CDECL_OBJFIRST);AngelScriptCheck(r); \
  r = as_engine->RegisterObjectMethod(#vec, #vec" opShrAssign(uint)", asFUNCTION((opShrAssign_scalar<vec, uint>)), asCALL_CDECL_OBJFIRST);AngelScriptCheck(r); \
  r = as_engine->RegisterObjectMethod(#vec, #vec" opShlAssign("#vec")", asFUNCTION((opShlAssign_vec<vec, vec>)), asCALL_CDECL_OBJFIRST);AngelScriptCheck(r); \
  r = as_engine->RegisterObjectMethod(#vec, #vec" opShrAssign("#vec")", asFUNCTION((opShrAssign_vec<vec, vec>)), asCALL_CDECL_OBJFIRST);AngelScriptCheck(r); \
  r = as_engine->RegisterObjectMethod(#vec, #vec" opAnd(int)", asFUNCTION((opAnd_scalar<vec, int>)), asCALL_CDECL_OBJFIRST);AngelScriptCheck(r); \
  r = as_engine->RegisterObjectMethod(#vec, #vec" opAnd(uint)", asFUNCTION((opAnd_scalar<vec, uint>)), asCALL_CDECL_OBJFIRST);AngelScriptCheck(r); \
  r = as_engine->RegisterObjectMethod(#vec, #vec" opAnd_r(int)", asFUNCTION((opAnd_scalar<vec, int>)), asCALL_CDECL_OBJFIRST);AngelScriptCheck(r); \
  r = as_engine->RegisterObjectMethod(#vec, #vec" opAnd_r(uint)", asFUNCTION((opAnd_scalar<vec, uint>)), asCALL_CDECL_OBJFIRST);AngelScriptCheck(r); \
  r = as_engine->RegisterObjectMethod(#vec, #vec" opAnd("#vec")", asFUNCTION((opAnd_vec<vec, vec>)), asCALL_CDECL_OBJFIRST);AngelScriptCheck(r); \
  r = as_engine->RegisterObjectMethod(#vec, #vec" opAndAssign(int)", asFUNCTION((opAndAssign_scalar<vec, int>)), asCALL_CDECL_OBJFIRST);AngelScriptCheck(r); \
  r = as_engine->RegisterObjectMethod(#vec, #vec" opAndAssign(uint)", asFUNCTION((opAndAssign_scalar<vec, uint>)), asCALL_CDECL_OBJFIRST);AngelScriptCheck(r); \
  r = as_engine->RegisterObjectMethod(#vec, #vec" opAndAssign("#vec")", asFUNCTION((opAndAssign_vec<vec, vec>)), asCALL_CDECL_OBJFIRST);AngelScriptCheck(r); \
  r = as_engine->RegisterObjectMethod(#vec, #vec" opOr(int)", asFUNCTION((opOr_scalar<vec, int>)), asCALL_CDECL_OBJFIRST);AngelScriptCheck(r); \
  r = as_engine->RegisterObjectMethod(#vec, #vec" opOr(uint)", asFUNCTION((opOr_scalar<vec, uint>)), asCALL_CDECL_OBJFIRST);AngelScriptCheck(r); \
  r = as_engine->RegisterObjectMethod(#vec, #vec" opOr_r(int)", asFUNCTION((opAnd_scalar<vec, int>)), asCALL_CDECL_OBJFIRST);AngelScriptCheck(r); \
  r = as_engine->RegisterObjectMethod(#vec, #vec" opOr_r(uint)", asFUNCTION((opAnd_scalar<vec, uint>)), asCALL_CDECL_OBJFIRST);AngelScriptCheck(r); \
  r = as_engine->RegisterObjectMethod(#vec, #vec" opOr("#vec")", asFUNCTION((opOr_vec<vec, vec>)), asCALL_CDECL_OBJFIRST);AngelScriptCheck(r); \
  r = as_engine->RegisterObjectMethod(#vec, #vec" opOrAssign(int)", asFUNCTION((opOrAssign_scalar<vec, int>)), asCALL_CDECL_OBJFIRST);AngelScriptCheck(r); \
  r = as_engine->RegisterObjectMethod(#vec, #vec" opOrAssign(uint)", asFUNCTION((opOrAssign_scalar<vec, uint>)), asCALL_CDECL_OBJFIRST);AngelScriptCheck(r); \
  r = as_engine->RegisterObjectMethod(#vec, #vec" opOrAssign("#vec")", asFUNCTION((opOrAssign_vec<vec, vec>)), asCALL_CDECL_OBJFIRST);AngelScriptCheck(r); \
  r = as_engine->RegisterObjectMethod(#vec, #vec" opXor(int)", asFUNCTION((opXor_scalar<vec, int>)), asCALL_CDECL_OBJFIRST);AngelScriptCheck(r); \
  r = as_engine->RegisterObjectMethod(#vec, #vec" opXor(uint)", asFUNCTION((opXor_scalar<vec, uint>)), asCALL_CDECL_OBJFIRST);AngelScriptCheck(r); \
  r = as_engine->RegisterObjectMethod(#vec, #vec" opXor_r(int)", asFUNCTION((opAnd_scalar<vec, int>)), asCALL_CDECL_OBJFIRST);AngelScriptCheck(r); \
  r = as_engine->RegisterObjectMethod(#vec, #vec" opXor_r(uint)", asFUNCTION((opAnd_scalar<vec, uint>)), asCALL_CDECL_OBJFIRST);AngelScriptCheck(r); \
  r = as_engine->RegisterObjectMethod(#vec, #vec" opXor("#vec")", asFUNCTION((opXor_vec<vec, vec>)), asCALL_CDECL_OBJFIRST);AngelScriptCheck(r); \
  r = as_engine->RegisterObjectMethod(#vec, #vec" opXorAssign(int)", asFUNCTION((opXorAssign_scalar<vec, int>)), asCALL_CDECL_OBJFIRST);AngelScriptCheck(r); \
  r = as_engine->RegisterObjectMethod(#vec, #vec" opXorAssign(uint)", asFUNCTION((opXorAssign_scalar<vec, uint>)), asCALL_CDECL_OBJFIRST);AngelScriptCheck(r); \
  r = as_engine->RegisterObjectMethod(#vec, #vec" opXorAssign("#vec")", asFUNCTION((opXorAssign_vec<vec, vec>)), asCALL_CDECL_OBJFIRST);AngelScriptCheck(r); \
 

#define REGISTER_INTEGER_OPERATORS(prefix, type) \
  REGISTER_NUMERIC_OPERATORS(prefix, type) \
  REGISTER_INTEGER_OPERATORS_(prefix##vec##2, type) \
  REGISTER_INTEGER_OPERATORS_(prefix##vec##3, type) \
  REGISTER_INTEGER_OPERATORS_(prefix##vec##4, type)

#define REGISTER_VEC2_(vec, type) \
  class HELPERCLASS_REGISTER_VEC2_##vec \
  { \
  public: \
    static void construct_type_type(vec##2 *v, type x, type y) \
    { \
      v->x = x; \
      v->y = y; \
    } \
    \
    static void construct_type(vec##2 *v, type x) \
    { \
      v->x = x; \
      v->y = x; \
    } \
    static void construct_vec2(vec##2 *v, const vec##2 &other) \
    { \
      *v = other; \
    } \
  }; \
  r = as_engine->RegisterObjectBehaviour(#vec"2", asBEHAVE_CONSTRUCT, "void f("#type" x, "#type" y)", asFUNCTION(&HELPERCLASS_REGISTER_VEC2_##vec::construct_type_type), asCALL_CDECL_OBJFIRST);AngelScriptCheck(r); \
  r = as_engine->RegisterObjectBehaviour(#vec"2", asBEHAVE_CONSTRUCT, "void f("#type" x)", asFUNCTION(&HELPERCLASS_REGISTER_VEC2_##vec::construct_type), asCALL_CDECL_OBJFIRST);AngelScriptCheck(r); \
  r = as_engine->RegisterObjectBehaviour(#vec"2", asBEHAVE_CONSTRUCT, "void f("#vec"2 &in v)", asFUNCTION(&HELPERCLASS_REGISTER_VEC2_##vec::construct_vec2), asCALL_CDECL_OBJFIRST);AngelScriptCheck(r); \
  \
  r = as_engine->RegisterObjectProperty(#vec"2", #type" x", asOFFSET(vec2, x));AngelScriptCheck(r); \
  r = as_engine->RegisterObjectProperty(#vec"2", #type" y", asOFFSET(vec2, y));AngelScriptCheck(r); \
  \
  r = as_engine->RegisterObjectProperty(#vec"2", #type" r", asOFFSET(vec2, r));AngelScriptCheck(r); \
  r = as_engine->RegisterObjectProperty(#vec"2", #type" g", asOFFSET(vec2, g));AngelScriptCheck(r); \
  \
  r = as_engine->RegisterObjectProperty(#vec"2", #type" s", asOFFSET(vec2, s));AngelScriptCheck(r); \
  r = as_engine->RegisterObjectProperty(#vec"2", #type" t", asOFFSET(vec2, t));AngelScriptCheck(r); \
 


#define REGISTER_VEC3_(vec, type) \
  class HELPERCLASS_REGISTER_VEC3_##vec \
  { \
  public: \
    static void construct_type_type_type(vec##3 *v, type x, type y, type z) \
    { \
      v->x = x; \
      v->y = y; \
      v->z = z; \
    } \
    \
    static void construct_type(vec##3 *v, type x) \
    { \
      v->x = x; \
      v->y = x; \
      v->z = x; \
    } \
    static void construct_vec3(vec##3 *v, const vec##3 &other) \
    { \
      *v = other; \
    } \
    static void construct_vec2_float(vec##3 *v, const vec##2 &a, float b) \
    { \
      v->x = a.x; \
      v->y = a.y; \
      v->z = b; \
    } \
    static void construct_float_vec2(vec##3 *v, float a, const vec##2 &b) \
    { \
      v->x = a; \
      v->y = b.x; \
      v->z = b.y;\
    } \
    static void construct_vec4(vec##3 *v, const vec##4 &a) \
    { \
      v->x = a.x; \
      v->y = a.y; \
      v->z = a.z; \
    } \
  }; \
  r = as_engine->RegisterObjectBehaviour(#vec"3", asBEHAVE_CONSTRUCT, "void f("#type" x, "#type" y, "#type" z)", asFUNCTION(&HELPERCLASS_REGISTER_VEC3_##vec::construct_type_type_type), asCALL_CDECL_OBJFIRST);AngelScriptCheck(r); \
  r = as_engine->RegisterObjectBehaviour(#vec"3", asBEHAVE_CONSTRUCT, "void f("#type" x)", asFUNCTION(&HELPERCLASS_REGISTER_VEC3_##vec::construct_type), asCALL_CDECL_OBJFIRST);AngelScriptCheck(r); \
  r = as_engine->RegisterObjectBehaviour(#vec"3", asBEHAVE_CONSTRUCT, "void f("#vec"3 &in v)", asFUNCTION(&HELPERCLASS_REGISTER_VEC3_##vec::construct_vec3), asCALL_CDECL_OBJFIRST);AngelScriptCheck(r); \
  r = as_engine->RegisterObjectBehaviour(#vec"3", asBEHAVE_CONSTRUCT, "void f("#vec"2 &in, float)", asFUNCTION(&HELPERCLASS_REGISTER_VEC3_##vec::construct_vec2_float), asCALL_CDECL_OBJFIRST);AngelScriptCheck(r); \
  r = as_engine->RegisterObjectBehaviour(#vec"3", asBEHAVE_CONSTRUCT, "void f(float, "#vec"2 &in)", asFUNCTION(&HELPERCLASS_REGISTER_VEC3_##vec::construct_float_vec2), asCALL_CDECL_OBJFIRST);AngelScriptCheck(r); \
  r = as_engine->RegisterObjectBehaviour(#vec"3", asBEHAVE_CONSTRUCT, "void f("#vec"4 &in)", asFUNCTION(&HELPERCLASS_REGISTER_VEC3_##vec::construct_vec4), asCALL_CDECL_OBJFIRST);AngelScriptCheck(r); \
  \
  r = as_engine->RegisterObjectProperty(#vec"3", #type" x", asOFFSET(vec3, x));AngelScriptCheck(r); \
  r = as_engine->RegisterObjectProperty(#vec"3", #type" y", asOFFSET(vec3, y));AngelScriptCheck(r); \
  r = as_engine->RegisterObjectProperty(#vec"3", #type" z", asOFFSET(vec3, z));AngelScriptCheck(r); \
  \
  r = as_engine->RegisterObjectProperty(#vec"3", #type" r", asOFFSET(vec3, r));AngelScriptCheck(r); \
  r = as_engine->RegisterObjectProperty(#vec"3", #type" g", asOFFSET(vec3, g));AngelScriptCheck(r); \
  r = as_engine->RegisterObjectProperty(#vec"3", #type" b", asOFFSET(vec3, b));AngelScriptCheck(r); \
  \
  r = as_engine->RegisterObjectProperty(#vec"3", #type" s", asOFFSET(vec3, s));AngelScriptCheck(r); \
  r = as_engine->RegisterObjectProperty(#vec"3", #type" t", asOFFSET(vec3, t));AngelScriptCheck(r); \
  r = as_engine->RegisterObjectProperty(#vec"3", #type" p", asOFFSET(vec3, p));AngelScriptCheck(r); \
 

#define REGISTER_VEC4_(vec, type) \
  class HELPERCLASS_REGISTER_VEC4_##vec \
  { \
  public: \
    static void construct_type_type_type_type(vec##4 *v, type x, type y, type z, type w) \
    { \
      v->x = x; \
      v->y = y; \
      v->z = z; \
      v->w = w; \
    } \
    \
    static void construct_type(vec##4 *v, type x) \
    { \
      v->x = x; \
      v->y = x; \
      v->z = x; \
      v->w = x; \
    } \
    static void construct_vec4(vec##4 *v, const vec##4 &other) \
    { \
      *v = other; \
    } \
    static void construct_vec3_float(vec##4 *v, const vec##3 &a, float b) \
    { \
      v->x = a.x; \
      v->y = a.y; \
      v->z = a.z; \
      v->w = b; \
    } \
    static void construct_float_vec3(vec##4 *v, float a, const vec##3 &b) \
    { \
      v->x = a; \
      v->y = b.x; \
      v->z = b.y;\
      v->w = b.z;\
    } \
    static void construct_vec2_vec2(vec##4 *v, const vec##2 &a, const vec##2 &b) \
    { \
      v->x = a.x; \
      v->y = a.y; \
      v->z = b.x;\
      v->w = b.y;\
    } \
    static void construct_vec2_float_float(vec##4 *v, const vec##2 &a, float b, float c) \
    { \
      v->x = a.x; \
      v->y = a.y; \
      v->z = b;\
      v->w = c;\
    } \
    static void construct_float_vec2_float(vec##4 *v, float a, const vec##2 &b, float c) \
    { \
      v->x = a; \
      v->y = b.x; \
      v->z = b.y;\
      v->w = c;\
    } \
    static void construct_float_float_vec2(vec##4 *v, float a, float b, const vec##2 &c) \
    { \
      v->x = a; \
      v->y = b; \
      v->z = c.x;\
      v->w = c.y;\
    } \
  }; \
  r = as_engine->RegisterObjectBehaviour(#vec"4", asBEHAVE_CONSTRUCT, "void f("#type" x, "#type" y, "#type" z, "#type" w)", asFUNCTION(&HELPERCLASS_REGISTER_VEC4_##vec::construct_type_type_type_type), asCALL_CDECL_OBJFIRST);AngelScriptCheck(r); \
  r = as_engine->RegisterObjectBehaviour(#vec"4", asBEHAVE_CONSTRUCT, "void f("#type" x)", asFUNCTION(&HELPERCLASS_REGISTER_VEC4_##vec::construct_type), asCALL_CDECL_OBJFIRST);AngelScriptCheck(r); \
  r = as_engine->RegisterObjectBehaviour(#vec"4", asBEHAVE_CONSTRUCT, "void f("#vec"4 &in v)", asFUNCTION(&HELPERCLASS_REGISTER_VEC4_##vec::construct_vec4), asCALL_CDECL_OBJFIRST);AngelScriptCheck(r); \
  r = as_engine->RegisterObjectBehaviour(#vec"4", asBEHAVE_CONSTRUCT, "void f("#vec"3 &in, float)", asFUNCTION(&HELPERCLASS_REGISTER_VEC4_##vec::construct_vec3_float), asCALL_CDECL_OBJFIRST);AngelScriptCheck(r); \
  r = as_engine->RegisterObjectBehaviour(#vec"4", asBEHAVE_CONSTRUCT, "void f(float, "#vec"3 &in)", asFUNCTION(&HELPERCLASS_REGISTER_VEC4_##vec::construct_float_vec3), asCALL_CDECL_OBJFIRST);AngelScriptCheck(r); \
  r = as_engine->RegisterObjectBehaviour(#vec"4", asBEHAVE_CONSTRUCT, "void f("#vec"2 &in, "#vec"2 &in)", asFUNCTION(&HELPERCLASS_REGISTER_VEC4_##vec::construct_vec2_vec2), asCALL_CDECL_OBJFIRST);AngelScriptCheck(r); \
  r = as_engine->RegisterObjectBehaviour(#vec"4", asBEHAVE_CONSTRUCT, "void f("#vec"2 &in, float, float)", asFUNCTION(&HELPERCLASS_REGISTER_VEC4_##vec::construct_vec2_float_float), asCALL_CDECL_OBJFIRST);AngelScriptCheck(r); \
  r = as_engine->RegisterObjectBehaviour(#vec"4", asBEHAVE_CONSTRUCT, "void f(float, "#vec"2 &in, float)", asFUNCTION(&HELPERCLASS_REGISTER_VEC4_##vec::construct_float_vec2_float), asCALL_CDECL_OBJFIRST);AngelScriptCheck(r); \
  r = as_engine->RegisterObjectBehaviour(#vec"4", asBEHAVE_CONSTRUCT, "void f(float, float, "#vec"2 &in)", asFUNCTION(&HELPERCLASS_REGISTER_VEC4_##vec::construct_float_float_vec2), asCALL_CDECL_OBJFIRST);AngelScriptCheck(r); \
  \
  r = as_engine->RegisterObjectProperty(#vec"4", #type" x", asOFFSET(vec4, x));AngelScriptCheck(r); \
  r = as_engine->RegisterObjectProperty(#vec"4", #type" y", asOFFSET(vec4, y));AngelScriptCheck(r); \
  r = as_engine->RegisterObjectProperty(#vec"4", #type" z", asOFFSET(vec4, z));AngelScriptCheck(r); \
  r = as_engine->RegisterObjectProperty(#vec"4", #type" w", asOFFSET(vec4, w));AngelScriptCheck(r); \
  \
  r = as_engine->RegisterObjectProperty(#vec"4", #type" r", asOFFSET(vec4, r));AngelScriptCheck(r); \
  r = as_engine->RegisterObjectProperty(#vec"4", #type" g", asOFFSET(vec4, g));AngelScriptCheck(r); \
  r = as_engine->RegisterObjectProperty(#vec"4", #type" b", asOFFSET(vec4, b));AngelScriptCheck(r); \
  r = as_engine->RegisterObjectProperty(#vec"4", #type" a", asOFFSET(vec4, a));AngelScriptCheck(r); \
  \
  r = as_engine->RegisterObjectProperty(#vec"4", #type" s", asOFFSET(vec4, s));AngelScriptCheck(r); \
  r = as_engine->RegisterObjectProperty(#vec"4", #type" t", asOFFSET(vec4, t));AngelScriptCheck(r); \
  r = as_engine->RegisterObjectProperty(#vec"4", #type" p", asOFFSET(vec4, p));AngelScriptCheck(r); \
  r = as_engine->RegisterObjectProperty(#vec"4", #type" q", asOFFSET(vec4, p));AngelScriptCheck(r); \
 


#define REGISTER_VEC(prefix, type) \
  REGISTER_VEC2_(prefix##vec, type) \
  REGISTER_VEC3_(prefix##vec, type) \
  REGISTER_VEC4_(prefix##vec, type)

void RegisterGlmVectors(asIScriptEngine* as_engine)
{
  int r;

  // Register the vec2 class
  // asOBJ_VALUE because it's a value type
  // asOBJ_POD  because it doesn't contain pointer or any other dangerous stuff :)
  // asOBJ_APP_CLASS because its a class
  // asOBJ_APP_CLASS_CONSTRUCTOR because it has a default contructor
  // asOBJ_APP_CLASS_ASSIGNMENT because it's able to assign vectors to other vectors
  // asOBJ_APP_CLASS_COPY_CONSTRUCTOR because it has a copy constructor
  // FLAGS may be asOBJ_APP_CLASS_ALLFLOATS if it consists only of floats
#define DECLARE_VECTOR_TYPE(name, FLAGS) \
  r = as_engine->RegisterObjectType(#name, \
                                    sizeof(name), \
                                    asOBJ_VALUE | \
                                    asOBJ_POD | \
                                    asOBJ_APP_CLASS | \
                                    asOBJ_APP_CLASS_CONSTRUCTOR | \
                                    asOBJ_APP_CLASS_ASSIGNMENT | \
                                    asOBJ_APP_CLASS_COPY_CONSTRUCTOR | \
                                    FLAGS); \
  AngelScriptCheck(r);

  DECLARE_VECTOR_TYPE(vec2, asOBJ_APP_CLASS_ALLFLOATS);
  DECLARE_VECTOR_TYPE(vec3, asOBJ_APP_CLASS_ALLFLOATS);
  DECLARE_VECTOR_TYPE(vec4, asOBJ_APP_CLASS_ALLFLOATS);
  DECLARE_VECTOR_TYPE(dvec2, 0);
  DECLARE_VECTOR_TYPE(dvec3, 0);
  DECLARE_VECTOR_TYPE(dvec4, 0);
  DECLARE_VECTOR_TYPE(bvec2, 0);
  DECLARE_VECTOR_TYPE(bvec3, 0);
  DECLARE_VECTOR_TYPE(bvec4, 0);
  DECLARE_VECTOR_TYPE(ivec2, asOBJ_APP_CLASS_ALLINTS);
  DECLARE_VECTOR_TYPE(ivec3, asOBJ_APP_CLASS_ALLINTS);
  DECLARE_VECTOR_TYPE(ivec4, asOBJ_APP_CLASS_ALLINTS);
  DECLARE_VECTOR_TYPE(uvec2, 0);
  DECLARE_VECTOR_TYPE(uvec3, 0);
  DECLARE_VECTOR_TYPE(uvec4, 0);

  REGISTER_VEC(,float);
  REGISTER_VEC(d,double);
  REGISTER_VEC(b,bool);
  REGISTER_VEC(i,int);
  REGISTER_VEC(u,uint);

  REGISTER_CAST(ivec, uvec);
  REGISTER_CAST(ivec,  vec);
  REGISTER_CAST(uvec,  vec);
  REGISTER_CAST(ivec, dvec);
  REGISTER_CAST(uvec, dvec);
  REGISTER_CAST( vec, dvec);

  REGISTER_REAL_OPERATORS(, float);
  REGISTER_REAL_OPERATORS(d, double);
  REGISTER_INTEGER_OPERATORS(i, int);
  REGISTER_INTEGER_OPERATORS(u, uint);
  REGISTER_BASIC_OPERATORS(b, bool);

}

} // namespace Angelscriptintegration

#pragma warning(pop)