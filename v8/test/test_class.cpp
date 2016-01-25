/*
 *
 *  Created on: 2016年1月23日
 *      Author: zhangyalei
 */

#include <V8_Class.h>
#include <V8_Property.h>
#include "test.h"

struct X
{
	int var = 1;

	int get() const { return var; }
	void set(int v) { var = v; }

	int fun(int x) { return var + x; }
	static int static_fun(int x) { return x; }
};

struct Y : X
{
	static int instance_count;

	explicit Y(int x) { var = x; ++instance_count; }
	~Y() { --instance_count; }
};

int Y::instance_count = 0;

namespace v8_wrap {
template<>
struct factory<Y>
{
	static Y* create(v8::Isolate*, int x) { return new Y(x); }
	static void destroy(v8::Isolate*, Y* object) { delete object; }
};
} // v8pp

void test_class()
{
	v8_wrap::context context;
	v8::Isolate* isolate = context.isolate();
	v8::HandleScope scope(isolate);

	v8_wrap::class_<X> X_class(isolate);
	X_class
		.ctor()
		.set_const("konst", 99)
		.set("var", &X::var)
		.set("rprop", v8_wrap::property(&X::get))
		.set("wprop", v8_wrap::property(&X::get, &X::set))
		.set("fun", &X::fun)
		.set("static_fun", &X::static_fun)
	;

	v8_wrap::class_<Y> Y_class(context.isolate());
	Y_class
		.inherit<X>()
		.ctor<int>()
		;

	context
		.set("X", X_class)
		.set("Y", Y_class)
		;

	check_eq("X object", run_script<int>(context, "x = new X(); x.konst + x.var"), 100);
	check_eq("X::rprop", run_script<int>(context, "x = new X(); x.rprop"), 1);
	check_eq("X::wprop", run_script<int>(context, "x = new X(); ++x.wprop"), 2);
	check_eq("X::fun(1)", run_script<int>(context, "x = new X(); x.fun(1)"), 2);
	check_eq("X::static_fun(1)", run_script<int>(context, "X.static_fun(3)"), 3);

	check_eq("Y object", run_script<int>(context, "y = new Y(-100); y.konst + y.var"), -1);

	Y y1(-1);
	v8::Handle<v8::Object> y1_obj = v8_wrap::class_<Y>::reference_external(context.isolate(), &y1);
	check("y1", v8_wrap::from_v8<Y*>(isolate, y1_obj) == &y1);
	check("y1_obj", v8_wrap::to_v8(isolate, y1) == y1_obj);

	Y* y2 = new Y(-2);
	v8::Handle<v8::Object> y2_obj = v8_wrap::class_<Y>::import_external(context.isolate(), y2);
	check("y2", v8_wrap::from_v8<Y*>(isolate, y2_obj) == y2);
	check("y2_obj", v8_wrap::to_v8(isolate, y2) == y2_obj);

	v8::Handle<v8::Object> y3_obj = v8_wrap::class_<Y>::create_object(context.isolate(), -3);
	Y* y3 = v8_wrap::class_<Y>::unwrap_object(isolate, y3_obj);
	check("y3", v8_wrap::from_v8<Y*>(isolate, y3_obj) == y3);
	check("y3_obj", v8_wrap::to_v8(isolate, y3) == y3_obj);
	check_eq("y3.var", y3->var, -3);

	run_script<int>(context, "for (i = 0; i < 10; ++i) new Y(i); i");
	check_eq("Y count", Y::instance_count, 10 + 4); // 10 + y + y1 + y2 + y3
	run_script<int>(context, "y = null; 0");

	v8_wrap::class_<Y>::unreference_external(isolate, &y1);
	check("unref y1", v8_wrap::from_v8<Y*>(isolate, y1_obj) == nullptr);
	check("unref y1_obj", v8_wrap::to_v8(isolate, y1).IsEmpty());
	y1_obj.Clear();

	v8_wrap::class_<Y>::destroy_object(isolate, y2);
	check("unref y2", v8_wrap::from_v8<Y*>(isolate, y2_obj) == nullptr);
	check("unref y2_obj", v8_wrap::to_v8(isolate, y2).IsEmpty());
	y2_obj.Clear();

	v8_wrap::class_<Y>::destroy_object(isolate, y3);
	check("unref y3", v8_wrap::from_v8<Y*>(isolate, y3_obj) == nullptr);
	check("unref y3_obj", v8_wrap::to_v8(isolate, y3).IsEmpty());
	y3_obj.Clear();

	std::string const v8_flags = "--expose_gc";
	v8::V8::SetFlagsFromString(v8_flags.data(), (int)v8_flags.length());
	context.isolate()->RequestGarbageCollectionForTesting(v8::Isolate::GarbageCollectionType::kFullGarbageCollection);

	check_eq("Y count after GC", Y::instance_count, 1); // y1

	v8_wrap::class_<Y>::destroy(isolate);
}
