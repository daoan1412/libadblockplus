#include <AdblockPlus.h>
#include <sstream>

#include "GlobalJsObject.h"
#include "Utils.h"

namespace
{
  v8::Handle<v8::Script> CompileScript(const std::string& source, const std::string& filename)
  {
    const v8::Handle<v8::String> v8Source = v8::String::New(source.c_str());
    if (filename.length())
    {
      const v8::Handle<v8::String> v8Filename = v8::String::New(filename.c_str());
      return v8::Script::Compile(v8Source, v8Filename);
    }
    else
      return v8::Script::Compile(v8Source);
  }

  void CheckTryCatch(const v8::TryCatch& tryCatch)
  {
    if (tryCatch.HasCaught())
      throw AdblockPlus::JsError(tryCatch.Exception(), tryCatch.Message());
  }

  std::string ExceptionToString(const v8::Handle<v8::Value> exception,
      const v8::Handle<v8::Message> message)
  {
    std::stringstream error;
    error << *v8::String::Utf8Value(exception);
    if (!message.IsEmpty())
    {
      error << " at ";
      error << *v8::String::Utf8Value(message->GetScriptResourceName());
      error << ":";
      error << message->GetLineNumber();
    }
    return error.str();
  }
}

AdblockPlus::JsError::JsError(const v8::Handle<v8::Value> exception,
    const v8::Handle<v8::Message> message)
  : std::runtime_error(ExceptionToString(exception, message))
{
}

AdblockPlus::JsEngine::JsEngine()
  : isolate(v8::Isolate::GetCurrent())
{
}

AdblockPlus::JsEnginePtr AdblockPlus::JsEngine::New(const AppInfo& appInfo)
{
  JsEnginePtr result(new JsEngine());

  const v8::Locker locker(result->isolate);
  const v8::HandleScope handleScope;

  result->context = v8::Context::New();
  AdblockPlus::GlobalJsObject::Setup(result, appInfo,
      JsValuePtr(new JsValue(result, result->context->Global())));
  return result;
}

AdblockPlus::JsValuePtr AdblockPlus::JsEngine::Evaluate(const std::string& source,
    const std::string& filename)
{
  const Context context(shared_from_this());
  const v8::TryCatch tryCatch;
  const v8::Handle<v8::Script> script = CompileScript(source, filename);
  CheckTryCatch(tryCatch);
  v8::Local<v8::Value> result = script->Run();
  CheckTryCatch(tryCatch);
  return JsValuePtr(new JsValue(shared_from_this(), result));
}

void AdblockPlus::JsEngine::Gc()
{
  while (!v8::V8::IdleNotification());
}

AdblockPlus::JsValuePtr AdblockPlus::JsEngine::NewValue(const std::string& val)
{
  const Context context(shared_from_this());
  return JsValuePtr(new JsValue(shared_from_this(),
      v8::String::New(val.c_str(), val.length())));
}

AdblockPlus::JsValuePtr AdblockPlus::JsEngine::NewValue(int64_t val)
{
  const Context context(shared_from_this());
  return JsValuePtr(new JsValue(shared_from_this(), v8::Number::New(val)));
}

AdblockPlus::JsValuePtr AdblockPlus::JsEngine::NewValue(bool val)
{
  const Context context(shared_from_this());
  return JsValuePtr(new JsValue(shared_from_this(), v8::Boolean::New(val)));
}

AdblockPlus::JsValuePtr AdblockPlus::JsEngine::NewObject()
{
  const Context context(shared_from_this());
  return JsValuePtr(new JsValue(shared_from_this(), v8::Object::New()));
}

AdblockPlus::JsValuePtr AdblockPlus::JsEngine::NewCallback(
    v8::InvocationCallback callback)
{
  const Context context(shared_from_this());

  // Note: we are leaking this weak pointer, no obvious way to destroy it when
  // it's no longer used
  std::tr1::weak_ptr<JsEngine>* data =
      new std::tr1::weak_ptr<JsEngine>(shared_from_this());
  v8::Local<v8::FunctionTemplate> templ = v8::FunctionTemplate::New(callback,
      v8::External::New(data));
  return JsValuePtr(new JsValue(shared_from_this(), templ->GetFunction()));
}

AdblockPlus::JsEnginePtr AdblockPlus::JsEngine::FromArguments(const v8::Arguments& arguments)
{
  const v8::Local<const v8::External> external =
      v8::Local<const v8::External>::Cast(arguments.Data());
  std::tr1::weak_ptr<JsEngine>* data =
      static_cast<std::tr1::weak_ptr<JsEngine>*>(external->Value());
  JsEnginePtr result = data->lock();
  if (!result)
    throw std::runtime_error("Oops, our JsEngine is gone, how did that happen?");
  return result;
}

AdblockPlus::JsValueList AdblockPlus::JsEngine::ConvertArguments(const v8::Arguments& arguments)
{
  const Context context(shared_from_this());
  JsValueList list;
  for (int i = 0; i < arguments.Length(); i++)
    list.push_back(JsValuePtr(new JsValue(shared_from_this(), arguments[i])));
  return list;
}

AdblockPlus::FileSystemPtr AdblockPlus::JsEngine::GetFileSystem()
{
  if (!fileSystem)
    fileSystem.reset(new DefaultFileSystem());
  return fileSystem;
}

void AdblockPlus::JsEngine::SetFileSystem(AdblockPlus::FileSystemPtr val)
{
  if (!val)
    throw std::runtime_error("FileSystem cannot be null");

  fileSystem = val;
}

AdblockPlus::WebRequestPtr AdblockPlus::JsEngine::GetWebRequest()
{
  if (!webRequest)
    webRequest.reset(new DefaultWebRequest());
  return webRequest;
}

void AdblockPlus::JsEngine::SetWebRequest(AdblockPlus::WebRequestPtr val)
{
  if (!val)
    throw std::runtime_error("WebRequest cannot be null");

  webRequest = val;
}

AdblockPlus::ErrorCallbackPtr AdblockPlus::JsEngine::GetErrorCallback()
{
  if (!errorCallback)
    errorCallback.reset(new DefaultErrorCallback());
  return errorCallback;
}

void AdblockPlus::JsEngine::SetErrorCallback(AdblockPlus::ErrorCallbackPtr val)
{
  if (!val)
    throw std::runtime_error("ErrorCallback cannot be null");

  errorCallback = val;
}

AdblockPlus::JsEngine::Context::Context(const JsEnginePtr jsEngine)
    : locker(jsEngine->isolate), handleScope(),
      contextScope(jsEngine->context)
{
}
