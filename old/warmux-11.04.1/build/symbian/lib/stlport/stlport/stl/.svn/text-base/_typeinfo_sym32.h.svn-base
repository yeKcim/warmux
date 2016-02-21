#ifndef _STLP_TYPEINFO_SYMBIAN32
#define _STLP_TYPEINFO_SYMBIAN32

#include <stl/_cstring.h>
#include <stl/_exception.h>

namespace std
{

#if defined (__WINSCW__)

class type_info
{
	const char  *tname;
	const void  *tbase;

public:
	bool operator== (const type_info& rhs) const {return static_cast<bool>(strcmp(tname, rhs.tname) == 0);}
	bool operator!= (const type_info& rhs) const {return static_cast<bool>(strcmp(tname, rhs.tname) != 0);}
	bool before (const type_info& rhs) const {return static_cast<bool>(strcmp(tname, rhs.tname) < 0);}
	const char* name () const {return tname;}

private:
	type_info (const type_info&);
	type_info& operator= (const type_info&);
};

#elif defined (__GCCE__)

namespace __cxxabiv1
{
  class __class_type_info;
}

class type_info {
public:
  virtual ~type_info ();

private:
  type_info& operator= (const type_info&);
  type_info (const type_info&);

protected:
  const char *__name;

protected:
  explicit type_info (const char *__n): __name (__n) { }

public:
  bool before (const type_info& __arg) const
    { return __name < __arg.__name; }
  const char* name () const
    { return __name; }
  bool operator== (const type_info& __arg) const
    { return __name == __arg.__name; }
  bool operator!= (const type_info& __arg) const
    { return !operator== (__arg); }

public:
  virtual bool __is_pointer_p () const;
  virtual bool __is_function_p () const;

  virtual bool __do_catch (const type_info *__thr_type, void **__thr_obj,
                         unsigned __outer) const;

  virtual bool __do_upcast (const __cxxabiv1::__class_type_info *__target,
                            void **__obj_ptr) const;
};

#else
#  error "typeinfo is not supported for this toolchain. Please disable it in config/stl_symbian.h"
#endif

class bad_cast : public exception {
public :
	bad_cast() _STLP_NOTHROW {}
	bad_cast(const bad_cast&) _STLP_NOTHROW {}
	bad_cast& operator=(const bad_cast&) _STLP_NOTHROW {return *this;}
	virtual ~bad_cast() _STLP_NOTHROW {}
	virtual const char* what() const _STLP_NOTHROW {return "bad_cast";}
};

class bad_typeid : public exception {
public:
	bad_typeid() _STLP_NOTHROW {}
	bad_typeid(const bad_typeid&) _STLP_NOTHROW {}
	bad_typeid& operator=(const bad_typeid&) _STLP_NOTHROW {return *this;}
	virtual ~bad_typeid() _STLP_NOTHROW {}
	virtual const char* what() const _STLP_NOTHROW {return "bad_typeid";}
};

}

#endif
