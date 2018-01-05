/*
 * Copyright 2004-present Facebook, Inc.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#include <folly/experimental/logging/StandardLogHandlerFactory.h>

#include <folly/MapUtil.h>
#include <folly/String.h>
#include <folly/experimental/logging/GlogStyleFormatter.h>
#include <folly/experimental/logging/StandardLogHandler.h>

using std::string;

namespace folly {

class GlogFormatterFactory
    : public StandardLogHandlerFactory::FormatterFactory {
 public:
  bool processOption(StringPiece /* name */, StringPiece /* value */) override {
    return false;
  }
  std::shared_ptr<LogFormatter> createFormatter() override {
    return std::make_shared<GlogStyleFormatter>();
  }
};

std::shared_ptr<StandardLogHandler> StandardLogHandlerFactory::createHandler(
    StringPiece type,
    WriterFactory* writerFactory,
    const Options& options) {
  std::unique_ptr<FormatterFactory> formatterFactory;

  // Get the log formatter type
  auto* formatterType = get_ptr(options, "formatter");
  if (!formatterType || *formatterType == "glog") {
    formatterFactory = std::make_unique<GlogFormatterFactory>();
  } else {
    throw std::invalid_argument(
        to<string>("unknown log formatter type \"", *formatterType, "\""));
  }

  // Process the log formatter and log handler options
  std::vector<string> errors;
  for (const auto& entry : options) {
    bool handled = false;
    try {
      // Allow both the formatterFactory and writerFactory to consume an
      // option.  In general they probably should have mutually exclusive
      // option names, but we don't give precedence to one over the other here.
      handled |= formatterFactory->processOption(entry.first, entry.second);
      handled |= writerFactory->processOption(entry.first, entry.second);
    } catch (const std::exception& ex) {
      errors.push_back(to<string>(
          "error processing option \"", entry.first, "\": ", ex.what()));
      continue;
    }

    // We explicitly processed the "formatter" option above.
    handled |= handled || (entry.first == "formatter");

    // Complain about unknown options.
    if (!handled) {
      errors.push_back(to<string>("unknown option \"", entry.first, "\""));
    }
  }

  if (!errors.empty()) {
    throw std::invalid_argument(join(", ", errors));
  }

  // Construct the formatter and writer
  auto formatter = formatterFactory->createFormatter();
  auto writer = writerFactory->createWriter();

  return std::make_shared<StandardLogHandler>(
      LogHandlerConfig{type, options}, formatter, writer);
}

} // namespace folly
