#ifndef LOGGER_H
#define LOGGER_H

class QString;

namespace Logger
{

enum class Mode {
    Normal,
    Debug,
};

void setup(const QString& logPath, Mode mode);

} // namespace

#endif /* LOGGER_H */
