/*
* Copyright (c) 2013 James Lomax (javajames64 @ http://forum.squirrel-lang.org/)
*
* This software is provided 'as-is', without any express or implied
* warranty. In no event will the authors be held liable for any damages
* arising from the use of this software.
*
* Permission is granted to anyone to use this software for any purpose,
* including commercial applications, and to alter it and redistribute it
* freely, subject to the following restrictions:
*
* 1. The origin of this software must not be misrepresented; you must not
* claim that you wrote the original software. If you use this software
* in a product, an acknowledgment in the product documentation would be
* appreciated but is not required.
*
* 2. Altered source versions must be plainly marked as such, and must not be
* misrepresented as being the original software.
*
* 3. This notice may not be removed or altered from any source
* distribution.
*/

#ifndef __SQUIRREL_EXTENSION_HEADER__
#define __SQUIRREL_EXTENSION_HEADER__

#include <vector>
#include <string.h>

#include "squirrel.h"
#include "sqrat.h"

/**
* @brief Sqrat extensions.
*
* <pre>
*  - Squirrel class management
*  - Variadic templates (sqrats endless copy&paste overloading of functions for
* parameters is a little messy IMO.)
*
* Usage at a glance:
*  - <VM> is the squirrel VM argument. Use Sqrat::DefaultVM::Set(<VM>) and this
*  argument can be ignored
*  - Create a class type from types defined in squirrel:
*      SQIClass myClass(<classname in root table>, <VM>);
*      SQIClass myClass(<sqrat object of this class>, <VM>);
*
*      EG:
*          SQIClass myClass("Entity");
*          SQIClass myClass(RootTable().GetSlot("Entity"));
*
*  - Bind a member to a handle lookup within the class:
*      myClass.bind(<handle index>, <member name>);
*      <handle index> = SQIClass::bind(<member name>);
*
*      EG:
*          myClass.bind(0, "move");
*          myClass.bind("move2") == 1;
*
*      Specifying the index manually will expand the handle lookup with nulls to cover it.
*
*  - Instantiate a new class in C++ from the class:
*      <SQIClassInstance> = SQIClass::New(<constructor parameters>);
*
*      EG:
*          SQIClassIndex object = myClass.New(5, 6, 7);
*
*  - Create a class instance from an existing Sqrat::Object:
*      <SQIClassInstance> = SQIClass::cast(<Sqrat::Object>);
*
*      EG:
*          SQIClassInstance object = myClass.cast(RootTable().GetSlot("anEntity"));
*
*  - Get the Sqrat::Object from a class instance:
*      <Sqrat::Object> = SQIClassInstance::get();
*
*      EG:
*          Sqrat::Object theEntity = object.get();
*
*  - Get the Sqrat::Object from a given handle or key:
*      <Sqrat::Object> = SQIClassInstance::get(<handle index>);
*      <Sqrat::Object> = SQIClassInstance::get(<key>);
*
*  - Get and cast the object from a given handle or key:
*      <T> = SQIClassInstance::cast<T>(<handle index>);
*      <T> = SQIClassInstance::cast<T>(<key>);
*
*      EG:
*          int x = object.cast<int>(4);
*          int x = object.cast<int>("xpos");
*
*  - Set a value or object to a given handle or key:
*      SQIClassInstance::set(<handle index>, <value>);
*      SQIClassInstance::set(<key>, <value>);
*
*  - Note that key lookup is slower than handle lookup, so as much as possible use pre-bound handles.
*
*  - Call the function at a given index:
*      SQIClassInstance::call(<handle index>);
*      SQIClassInstance::call(<handle index>, arguments...);
*      <T> = SQIClassInstance::callr<T>(<handle index>);
*      <T> = SQIClassInstance::callr<T>(<handle index>, arguments...);
*
*      EG:
*          object->call(0);
*          object->call(0, blah, blah, blah); //Any squirrel types accepted
*          int i = object.call<int>(0);
*          int i = object.call<int>(0, blah, blah, blah);
*  - Objects and functions by string name:
*      - Objects and functions by string value should be retrieved from
*      the Sqrat::Object.
*      - The SQIClassInstance::get() function will retrieve the Sqrat::Object
*      cast of the sqext::SQIClassInstance:
*          Sqrat::Object asObject = object->get();
*          //From this the Sqrat functionality can be used
*          asObject.GetSlot("member");
*      - There are a few convenience functions provided for ease of reading:
*          SQIClassInstance::get(<key>) -> SQIClassInstance::get().GetSlot(<key>);
*          SQIClassInstance::cast<T>(<key>) -> SQIClassInstance::get().GetSlot(<key>).Cast<T>();
*          SQIClassInstance::call(<key>, <args...>) -> Sqrat::Function(SQIClassInstance::get(), <key>)(<args...>);
* </pre>
*/
namespace sqext {

	/**
	* @brief Variadic argument processor.
	* @param vm    Squirrel VM
	* @param arg   First argument for processing
	* @tparam Arg  Type of the first argument
	* @return Argument number
	*/
	template <class Arg>
	inline int pushArgs(HSQUIRRELVM vm, Arg arg) {
		Sqrat::PushVar(vm, arg);
		return 1;
	}

	/**
	* @brief Variadic argument processor. This is overloading to exploit tail
	* recursion and variadics to achieve unlimited arguments.
	* @param vm    Squirrel VM
	* @param arg   First argument for processing
	* @param args  Rest of the arguments
	* @tparam Arg  Type of the first argument
	* @tparam Args Type of the rest of the arguments
	* @return Argument number
	*/
	template <class Arg, class... Args>
	inline int pushArgs(HSQUIRRELVM vm, Arg arg, Args... args) {
		return pushArgs(vm, arg) + pushArgs(vm, args...);
	}

	/**
	* @brief Constructor allocator for variadic constructor arguments with Sqrat::Class.
	* When constructing a class, pass this as the allocator template parameter with the
	* parameters specifying the parameters of the visible constructor you wish to expose
	* to the script. For example, if you have a class Point, and wish to expose the
	*  Point(int x, int y);
	* constructor, you can do so on creating the sqrat class:
	*  Sqrat::Class<Point, sqext::ConstAlloc<Point, int, int> > pointClass;
	* @tparam C    Class to allocate for
	* @tparam Args Argument types.
	*/
	template <class C, class... Args>
	class ConstAlloc : public Sqrat::DefaultAllocator<C> {
	public:
		/**
		* @brief Internal function for creating a new instance of the class.
		*/
		static SQInteger New(HSQUIRRELVM vm) {
			int index = 2;
			C *instance = new C(Sqrat::Var<Args>(vm, index++).value...);
			sq_setinstanceup(vm, 1, instance);
			sq_setreleasehook(vm, 1, &Sqrat::DefaultAllocator<C>::Delete);
			return 0;
		}
	};

	class SQIClass;

	/**
	* @brief Squirrel class instance. This handles fast access to handled members.
	* The handles of a SQI class instance are immutable.
	*/
	class SQIClassInstance {
	public:
		/**
		* @brief SQIClassInstance constructor
		* @param v Squirrel VM
		* @param o Object
		* @param handles   Handles of this object
		*/
		SQIClassInstance(HSQUIRRELVM v, HSQOBJECT o, SQIClass &ct) : vm(v), object(o), classt(ct) {
			sq_addref(vm, &object);
		}

		/**
		* @brief SQIClassInstance copy constructor
		*/
		SQIClassInstance(const SQIClassInstance &other) : vm(other.vm), object(other.object), classt(other.classt) {
			sq_addref(vm, &object);
		}

		~SQIClassInstance() {
			sq_release(vm, &object);
			sq_resetobject(&object);
		}

		/**
		* @brief Get the object as a sqrat object
		* @return The sqrat object
		*/
		Sqrat::Object get() {
			return Sqrat::Object(object, vm);
		}

		/**
		* @brief Get the object by its handle
		* @param index Index of the handle
		* @return The sqrat object associated with this handle
		*/
		inline Sqrat::Object get(int index);

		/**
		* @brief Get the object by its handle
		* @param index Index of the handle
		* @tparam T    Type to cast to
		* @return The sqrat object associated with this handle
		*/
		template <class T>
		inline T cast(int index) {
			return get(index).Cast<T>();
		}

		/**
		* @brief Get an member object by it's key. This is a convenience function.
		* @param key   Member name
		* @return The Sqrat::Object under this key
		*/
		inline Sqrat::Object get(const SQChar *key) {
			return get().GetSlot(key);
		}

		/**
		* @brief Get an member object by it's key. This is a convenience function.
		* @param key   Member name
		* @tparam T    Type to cast to
		* @return The object under this key cast to type T
		*/
		template <class T>
		inline T cast(const SQChar *key) {
			return get().GetSlot(key).Cast<T>();
		}

		/**
		* @brief Set the object at a given index
		* @param index Index of the object handle
		* @param o Object
		* @tparam T    Object type
		*/
		template <class T>
		inline void set(int index, T o);

		/**
		* @brief Set the object at a given index
		* @param key   Object name
		* @param o Object
		* @tparam T    Object type
		*/
		template <class T>
		inline void set(const SQChar *key, T o) {
			sq_pushobject(vm, object);
			sq_pushstring(vm, key, -1);
			pushArgs(vm, o);

			if (SQ_FAILED(sq_set(vm, -3)))
				throw Sqrat::Exception(Sqrat::LastErrorString(vm));

			sq_pop(vm, 1);
		}

		/**
		* @brief Call a function by its handle
		* @param index Index of the handle
		*/
		void call(int index) {
			sq_pushobject(vm, get(index).GetObject());
			sq_pushobject(vm, object);

			if (SQ_FAILED(sq_call(vm, 1, false, true)))
				throw Sqrat::Exception(Sqrat::LastErrorString(vm));

			sq_pop(vm, 1);
		}

		/**
		* @brief Call a function by its handle
		* @param index Index of the handle
		* @param args  Arguments to pass to the function
		* @tparam Args Argument type
		*/
		template <class... Args>
		void call(int index, Args... args) {
			sq_pushobject(vm, get(index).GetObject());
			sq_pushobject(vm, object);

			int argc = pushArgs(vm, args...);

			if (SQ_FAILED(sq_call(vm, 1 + argc, false, true)))
				throw Sqrat::Exception(Sqrat::LastErrorString(vm));

			sq_pop(vm, 1);
		}

		/**
		* @brief Call a function by its handle. Calls with return type parameter
		* which must be specified (e.g. obj.callr<int>(0);)
		* @param index Index of the handle
		* @tparam RT   Return type (must be specified)
		* @return The return of the function
		*/
		template <class RT>
		RT callr(int index) {
			sq_pushobject(vm, get(index).GetObject());
			sq_pushobject(vm, object);

			if (SQ_FAILED(sq_call(vm, 1, true, true)))
				throw Sqrat::Exception(Sqrat::LastErrorString(vm));

			HSQOBJECT retObj;
			sq_getstackobj(vm, -1, &retObj);
			Sqrat::Object obj(retObj, vm);
			sq_pop(vm, 2);

			return obj.Cast<RT>();
		}

		/**
		* @brief Call a function by its handle. Calls with return type parameter
		* which must be specified (e.g. obj.callr<int>(0);)
		* @param index Index of the handle
		* @param args  Arguments to pass to the function
		* @tparam RT   Return type (must be specified)
		* @tparam Args Argument type
		* @return The return of the function
		*/
		template <class RT, class... Args>
		RT callr(int index, Args... args) {
			sq_pushobject(vm, get(index).GetObject());
			sq_pushobject(vm, object);

			int argc = pushArgs(vm, args...);

			if (SQ_FAILED(sq_call(vm, 1 + argc, true, true)))
				throw Sqrat::Exception(Sqrat::LastErrorString(vm));

			HSQOBJECT retObj;
			sq_getstackobj(vm, -1, &retObj);
			Sqrat::Object obj(retObj, vm);
			sq_pop(vm, 2);

			return obj.Cast<RT>();
		}

		/**
		* @brief Call a function by name
		* @param key   Key value of the member function
		*/
		inline void call(const SQChar *key) {
			Sqrat::Function(get(), key)();
		}

		/**
		* @brief Call a function by name
		* @param key   Key value of the member function
		* @param args  Arguments to pass to the function
		* @tparam Args Argument type
		*/
		template <class... Args>
		inline void call(const SQChar *key, Args... args) {
			Sqrat::Function(get(), key)(args...);
		}

		/**
		* @brief Call a function by name. Calls with return type parameter
		* which must be specified (e.g. obj.callr<int>("func");)
		* @param key   Key value of the member function
		* @tparam RT   Return type (must be specified)
		* @return The return of the function
		*/
		template <class RT>
		inline RT callr(const SQChar *key) {
			return Sqrat::Function(get(), key).Evaluate<RT>();
		}

		/**
		* @brief Call a function by name. Calls with return type parameter
		* which must be specified (e.g. obj.callr<int>("func");)
		* @param key   Key value of the member function
		* @param args  Arguments to pass to the function
		* @tparam RT   Return type (must be specified)
		* @tparam Args Argument type
		* @return The return of the function
		*/
		template <class RT, class... Args>
		inline RT callr(const SQChar *key, Args... args) {
			return Sqrat::Function(get(), key).Evaluate<RT>(args...);
		}

	private:
		//Squirrel virtual machine
		const HSQUIRRELVM vm;

		HSQOBJECT object;

		//SQIClass reference
		SQIClass &classt;

	};

	/**
	* @brief Squirrel internal class. This class handles classes defined in scripts
	*/
	class SQIClass {
	public:
		/**
		* @brief SQIClass constructor
		* @param cobj  Class object
		* @param v Squirrel VM
		*/
		SQIClass(Sqrat::Object cobj, HSQUIRRELVM v = Sqrat::DefaultVM::Get()) : classobj(cobj), vm(v) {}

		/**
		* @brief SQIClass constructor
		* @param cobj  Class object
		* @param v Squirrel VM
		*/
		SQIClass(const SQChar *key, HSQUIRRELVM v = Sqrat::DefaultVM::Get()) : vm(v) {
			classobj = Sqrat::RootTable(vm).GetSlot(key);
		}

		/**
		* @brief Instantiate a new object of this class with all the handles of this type.
		* @return A class instance of this type
		*/
		SQIClassInstance New() {
			HSQOBJECT pclass = classobj.GetObject();
			HSQOBJECT newObject;

			//Call the constructor
			sq_pushobject(vm, pclass);
			sq_pushnull(vm); //this === null (thanks fagiano :D)

			if (SQ_FAILED(sq_call(vm, 1, true, true)))
				throw Sqrat::Exception(Sqrat::LastErrorString(vm));

			sq_getstackobj(vm, -1, &newObject);

			SQIClassInstance object(vm, newObject, *this);
			sq_pop(vm, 2);

			return object;
		}

		SQIClassInstance* NewPointer() {
			HSQOBJECT pclass = classobj.GetObject();
			HSQOBJECT newObject;

			//Call the constructor
			sq_pushobject(vm, pclass);
			sq_pushnull(vm); //this === null (thanks fagiano :D)

			if (SQ_FAILED(sq_call(vm, 1, true, true)))
				throw Sqrat::Exception(Sqrat::LastErrorString(vm));

			sq_getstackobj(vm, -1, &newObject);

			SQIClassInstance* object = new SQIClassInstance(vm, newObject, *this);
			sq_pop(vm, 2);

			return object;
		}

		/**
		* @brief Instantiate a new object of this class with all the handles of this type.
		* @param args  Arguments to instantiate with
		* @tparam Args Argument parameter types
		* @return A class instance of this type
		*/
		template <class... Args>
		SQIClassInstance New(Args... args) {
			HSQOBJECT pclass = classobj.GetObject();
			HSQOBJECT newObject;

			//Call the constructor
			sq_pushobject(vm, pclass);
			sq_pushnull(vm); //this === null

			int argc = pushArgs(vm, args...);

			if (SQ_FAILED(sq_call(vm, 1 + argc, true, true)))
				throw Sqrat::Exception(Sqrat::LastErrorString(vm));

			sq_getstackobj(vm, -1, &newObject);

			SQIClassInstance object(vm, newObject, *this);
			sq_pop(vm, 2);

			return object;
		}

		/**
		* @brief Bind a member handle of member "key" to an index. This will bind
		* to a specific index and expand the handles vector to account for this.
		* This therefore should only be used on instantiation. Otherwise use
		* the single parameter version which allocates a single new member handle.
		* @param index Lookup index of the member handle
		* @param key   Key value of the member handle within the squirrel VM
		*/
		void bind(int index, const SQChar *key) {
			if (index >= handles.size())
				handles.resize(index + 1);

			HSQMEMBERHANDLE handle;

			sq_pushobject(vm, classobj.GetObject());
			sq_pushstring(vm, key, -1);

			if (SQ_FAILED(sq_getmemberhandle(vm, -2, &handle)))
				throw Sqrat::Exception(Sqrat::LastErrorString(vm));

			sq_pop(vm, 1);

			handles[index] = handle;
		}

		/**
		* @brief Bind a member handle of member "key" to an index. This will bind
		* a new member handle to the end of the handles vector and get it's index.
		* @param key   Key value of the member handle within the squirrel VM
		* @return Index of the member handle
		*/
		int bind(const SQChar *key) {
			int index = handles.size();
			bind(index, key);
			return index;
		}

		/**
		* @brief Cast a regular sqrat object to an SQIClassInstance
		* @param obj   Sqrat object
		* @return The class instance
		*/
		SQIClassInstance cast(Sqrat::Object obj) {
			return SQIClassInstance(vm, obj.GetObject(), *this);
		}

		/**
		* @brief Get a handle at a given index
		* @param index Index of the handle
		* @return The squirrel member handle object
		*/
		HSQMEMBERHANDLE getHandle(int index) const {
			return handles[index];
		}

	private:
		//Sqrat class object type
		Sqrat::Object classobj;

		//Squirrel VM
		const HSQUIRRELVM vm;

		//Member handles lookup
		std::vector<HSQMEMBERHANDLE> handles;

	};

	Sqrat::Object SQIClassInstance::get(int index) {
		HSQOBJECT obj;
		sq_pushobject(vm, object);

		HSQMEMBERHANDLE handle = classt.getHandle(index);
		if (SQ_FAILED(sq_getbyhandle(vm, -1, &handle)))
			throw Sqrat::Exception(Sqrat::LastErrorString(vm));

		sq_getstackobj(vm, -1, &obj);
		Sqrat::Object sobj(obj, vm);

		sq_pop(vm, 2);

		return sobj;
	}

	template <class T>
	inline void SQIClassInstance::set(int index, T o) {
		sq_pushobject(vm, object);

		pushArgs(vm, o);

		HSQMEMBERHANDLE handle = classt.getHandle(index);
		if (SQ_FAILED(sq_setbyhandle(vm, -2, &handle)))
			throw Sqrat::Exception(Sqrat::LastErrorString(vm));

		sq_pop(vm, 1);
	}

}

#endif // __SQUIRREL_EXTENSION_HEADER__