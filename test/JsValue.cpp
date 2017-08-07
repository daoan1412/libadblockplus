/*
 * This file is part of Adblock Plus <https://adblockplus.org/>,
 * Copyright (C) 2006-2017 eyeo GmbH
 *
 * Adblock Plus is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 3 as
 * published by the Free Software Foundation.
 *
 * Adblock Plus is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Adblock Plus.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "BaseJsTest.h"

namespace
{
  class JsValueTest : public BaseJsTest
  {
  };
}

TEST_F(JsValueTest, UndefinedValue)
{
  auto value = platform->GetJsEngine()->Evaluate("undefined");
  ASSERT_TRUE(value.IsUndefined());
  ASSERT_FALSE(value.IsNull());
  ASSERT_FALSE(value.IsString());
  ASSERT_FALSE(value.IsBool());
  ASSERT_FALSE(value.IsNumber());
  ASSERT_FALSE(value.IsObject());
  ASSERT_FALSE(value.IsArray());
  ASSERT_FALSE(value.IsFunction());
  ASSERT_EQ("undefined", value.AsString());
  ASSERT_FALSE(value.AsBool());
  ASSERT_ANY_THROW(value.AsList());
  ASSERT_ANY_THROW(value.GetProperty("foo"));
  ASSERT_ANY_THROW(value.SetProperty("foo", false));
  ASSERT_ANY_THROW(value.GetClass());
  ASSERT_ANY_THROW(value.GetOwnPropertyNames());
  ASSERT_ANY_THROW(value.Call());
}

TEST_F(JsValueTest, NullValue)
{
  auto value = platform->GetJsEngine()->Evaluate("null");
  ASSERT_FALSE(value.IsUndefined());
  ASSERT_TRUE(value.IsNull());
  ASSERT_FALSE(value.IsString());
  ASSERT_FALSE(value.IsBool());
  ASSERT_FALSE(value.IsNumber());
  ASSERT_FALSE(value.IsObject());
  ASSERT_FALSE(value.IsArray());
  ASSERT_FALSE(value.IsFunction());
  ASSERT_EQ("null", value.AsString());
  ASSERT_FALSE(value.AsBool());
  ASSERT_ANY_THROW(value.AsList());
  ASSERT_ANY_THROW(value.GetProperty("foo"));
  ASSERT_ANY_THROW(value.SetProperty("foo", false));
  ASSERT_ANY_THROW(value.GetClass());
  ASSERT_ANY_THROW(value.GetOwnPropertyNames());
  ASSERT_ANY_THROW(value.Call());
}

TEST_F(JsValueTest, StringValue)
{
  auto value = platform->GetJsEngine()->Evaluate("'123'");
  ASSERT_FALSE(value.IsUndefined());
  ASSERT_FALSE(value.IsNull());
  ASSERT_TRUE(value.IsString());
  ASSERT_FALSE(value.IsBool());
  ASSERT_FALSE(value.IsNumber());
  ASSERT_FALSE(value.IsObject());
  ASSERT_FALSE(value.IsArray());
  ASSERT_FALSE(value.IsFunction());
  ASSERT_EQ("123", value.AsString());
  ASSERT_EQ(123, value.AsInt());
  ASSERT_TRUE(value.AsBool());
  ASSERT_ANY_THROW(value.AsList());
  ASSERT_ANY_THROW(value.GetProperty("foo"));
  ASSERT_ANY_THROW(value.SetProperty("foo", false));
  ASSERT_ANY_THROW(value.GetClass());
  ASSERT_ANY_THROW(value.GetOwnPropertyNames());
  ASSERT_ANY_THROW(value.Call());
}

TEST_F(JsValueTest, IntValue)
{
  auto value = platform->GetJsEngine()->Evaluate("12345678901234");
  ASSERT_FALSE(value.IsUndefined());
  ASSERT_FALSE(value.IsNull());
  ASSERT_FALSE(value.IsString());
  ASSERT_FALSE(value.IsBool());
  ASSERT_TRUE(value.IsNumber());
  ASSERT_FALSE(value.IsObject());
  ASSERT_FALSE(value.IsArray());
  ASSERT_FALSE(value.IsFunction());
  ASSERT_EQ("12345678901234", value.AsString());
  ASSERT_EQ(12345678901234, value.AsInt());
  ASSERT_TRUE(value.AsBool());
  ASSERT_ANY_THROW(value.AsList());
  ASSERT_ANY_THROW(value.GetProperty("foo"));
  ASSERT_ANY_THROW(value.SetProperty("foo", false));
  ASSERT_ANY_THROW(value.GetClass());
  ASSERT_ANY_THROW(value.GetOwnPropertyNames());
  ASSERT_ANY_THROW(value.Call());
}

TEST_F(JsValueTest, BoolValue)
{
  auto value = platform->GetJsEngine()->Evaluate("true");
  ASSERT_FALSE(value.IsUndefined());
  ASSERT_FALSE(value.IsNull());
  ASSERT_FALSE(value.IsString());
  ASSERT_TRUE(value.IsBool());
  ASSERT_FALSE(value.IsNumber());
  ASSERT_FALSE(value.IsObject());
  ASSERT_FALSE(value.IsArray());
  ASSERT_FALSE(value.IsFunction());
  ASSERT_EQ("true", value.AsString());
  ASSERT_TRUE(value.AsBool());
  ASSERT_ANY_THROW(value.AsList());
  ASSERT_ANY_THROW(value.GetProperty("foo"));
  ASSERT_ANY_THROW(value.SetProperty("foo", false));
  ASSERT_ANY_THROW(value.GetClass());
  ASSERT_ANY_THROW(value.GetOwnPropertyNames());
  ASSERT_ANY_THROW(value.Call());
}

TEST_F(JsValueTest, ObjectValue)
{
  const std::string source("\
    function Foo() {\
      this.x = 2;\
      this.toString = function() {return 'foo';};\
      this.valueOf = function() {return 123;};\
    };\
    new Foo()");
  auto value = platform->GetJsEngine()->Evaluate(source);
  ASSERT_FALSE(value.IsUndefined());
  ASSERT_FALSE(value.IsNull());
  ASSERT_FALSE(value.IsString());
  ASSERT_FALSE(value.IsBool());
  ASSERT_FALSE(value.IsNumber());
  ASSERT_TRUE(value.IsObject());
  ASSERT_FALSE(value.IsArray());
  ASSERT_FALSE(value.IsFunction());
  ASSERT_EQ("foo", value.AsString());
  ASSERT_EQ(123, value.AsInt());
  ASSERT_TRUE(value.AsBool());
  ASSERT_ANY_THROW(value.AsList());
  ASSERT_EQ(2, value.GetProperty("x").AsInt());
  value.SetProperty("x", 12);
  ASSERT_EQ(12, value.GetProperty("x").AsInt());
  value.SetProperty("x", platform->GetJsEngine()->NewValue(15));
  ASSERT_EQ(15, value.GetProperty("x").AsInt());
  ASSERT_EQ("Foo", value.GetClass());
  ASSERT_EQ(3u, value.GetOwnPropertyNames().size());
  ASSERT_ANY_THROW(value.Call());
}

TEST_F(JsValueTest, ArrayValue)
{
  auto value = platform->GetJsEngine()->Evaluate("[5,8,12]");
  ASSERT_FALSE(value.IsUndefined());
  ASSERT_FALSE(value.IsNull());
  ASSERT_FALSE(value.IsString());
  ASSERT_FALSE(value.IsBool());
  ASSERT_FALSE(value.IsNumber());
  ASSERT_TRUE(value.IsObject());
  ASSERT_TRUE(value.IsArray());
  ASSERT_FALSE(value.IsFunction());
  ASSERT_EQ("5,8,12", value.AsString());
  ASSERT_TRUE(value.AsBool());
  ASSERT_EQ(3u, value.AsList().size());
  ASSERT_EQ(8, value.AsList()[1].AsInt());
  ASSERT_EQ(3, value.GetProperty("length").AsInt());
  ASSERT_EQ("Array", value.GetClass());
  ASSERT_ANY_THROW(value.Call());
}

TEST_F(JsValueTest, FunctionValue)
{
  auto value = platform->GetJsEngine()->Evaluate("(function(foo, bar) {return this.x + '/' + foo + '/' + bar;})");
  ASSERT_FALSE(value.IsUndefined());
  ASSERT_FALSE(value.IsNull());
  ASSERT_FALSE(value.IsString());
  ASSERT_FALSE(value.IsBool());
  ASSERT_FALSE(value.IsNumber());
  ASSERT_TRUE(value.IsObject());
  ASSERT_FALSE(value.IsArray());
  ASSERT_TRUE(value.IsFunction());
  ASSERT_TRUE(value.AsBool());
  ASSERT_ANY_THROW(value.AsList());
  ASSERT_EQ(2, value.GetProperty("length").AsInt());

  auto thisPtr = platform->GetJsEngine()->Evaluate("({x:2})");
  AdblockPlus::JsValueList params;
  params.push_back(platform->GetJsEngine()->NewValue(5));
  params.push_back(platform->GetJsEngine()->NewValue("xyz"));
  ASSERT_EQ("2/5/xyz", value.Call(params, thisPtr).AsString());
}

TEST_F(JsValueTest, JsValueCallSingleArg)
{
  auto func = platform->GetJsEngine()->Evaluate("(function(arg) {return arg * 2;})");
  EXPECT_EQ(10, func.Call(platform->GetJsEngine()->NewValue(5)).AsInt());
}

TEST_F(JsValueTest, ThrowingCoversion)
{
  const std::string source("\
    function Foo() {\
      this.toString = function() {throw 'test1';};\
      this.valueOf = function() {throw 'test2';};\
    };\
    new Foo()");
  auto value = platform->GetJsEngine()->Evaluate(source);
  ASSERT_EQ("", value.AsString());
  ASSERT_EQ(0, value.AsInt());
}
