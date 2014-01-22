//
//  Created by Alexander Simakhin
//

#pragma once

#include "Utils/Hasher.h"

#define ATOMIC_INC(ptr) __sync_fetch_and_add ((ptr), 1)
#define ATOMIC_DEC(ptr) __sync_fetch_and_add ((ptr), -1)

#define CALL_ONCE( functionString )\
static struct OnceCaller { OnceCaller(){ functionString; } }onceCallerInstance;\

#define CALL_ONCE_PREFIX( functionString, prefix )\
static struct OnceCaller##prefix { OnceCaller##prefix(){ functionString; } }onceCallerInstance##prefix;\

#define CALL_ONCE_ARG( functionName, arg )\
static struct OnceCaller { OnceCaller(){ functionName(arg); } } onceCallerInstance;

#define CALL_ONCE_TYPE_TYPE_PREFIX( functionName, type1, type2, prefix )\
static struct OnceCaller##prefix { OnceCaller##prefix(){ functionName<type1, type2>(); } }onceCallerInstance##prefix;\


#define SINGLETON_UNRELEASABLE( varClassName )\
public: static inline varClassName* Instance()\
{\
static varClassName instance;\
return &instance;\
}\

#define SINGLETON_RELEASABLE( varClassName )\
private: static inline varClassName* ModifyInstance( bool createInstance )\
{\
static varClassName* instance = NULL;\
\
if( createInstance )\
{\
if(!instance)\
{\
instance = new varClassName();\
}\
return instance;\
}\
if(instance)\
{\
delete instance;\
instance = NULL;\
}\
return instance;\
}\
\
public: static inline varClassName* Instance()\
{\
return ModifyInstance( true );\
}\
\
static inline void Release()\
{\
ModifyInstance( false );\
}\

#define ARRAY_SIZE(array) (sizeof(array)/sizeof(*array))

#define PROPERTY_GET(varType, varName, funName)\
protected: varType varName;\
public: inline varType Get##funName(void) const { return varName; }

#define PROPERTYCONST_GET(varType, varName, funName)\
protected: const varType varName;\
public: inline const varType Get##funName(void) const { return varName; }

#define PROPERTY_GETCONST(varType, varName, funName)\
protected: varType varName;\
public: inline const varType Get##funName(void) const { return varName; }

#define PROPERTY_GETREF_NOTCONST(varType, varName, funName)\
protected: varType varName;\
public: inline varType& Get##funName(void) { return varName; }

#define PROPERTY_GETREF(varType, varName, funName)\
protected: varType varName;\
public: inline const varType& Get##funName(void) const { return varName; }

#define PROPERTY_GET_SET(varType, varName, funName)\
protected: varType varName;\
public: inline varType Get##funName(void) const { return varName; }\
public: inline void Set##funName(varType var) { varName = var; }

#define PROPERTYMUTABLE_GET_SET(varType, varName, funName)\
protected: mutable varType varName;\
public: inline varType Get##funName(void) const { return varName; }\
public: inline void Set##funName(varType var) const { varName = var; }

#define PROPERTY_GETREF_SETREF(varType, varName, funName)\
protected: varType varName;\
public: inline const varType& Get##funName(void) const { return varName; }\
public: inline void Set##funName( const varType& var) { varName = var; }

#define PROPERTY_PROTECTED(varType, varName)\
protected: varType varName;

#define PROPERTY_PRIVATE(varType, varName)\
private: varType varName;

namespace Core
{    
    typedef int HashId;
}

#define LOG(format, ...)  CCLOG(format, ##__VA_ARGS__)

