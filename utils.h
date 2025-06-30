#ifndef UTILS_H
#define UTILS_H

#include <QString>
#include <QFile>
#include <QIODevice>
#include <QByteArray>
#include <QJsonParseError>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QDebug>
#include <QVariant>

QVariant readJsonProperty(const QString& filePath, const QString& propertyName);
bool setJsonProperty(const QString& filePath, const QString& propertyName, const QVariant& value);

#endif // UTILS_H