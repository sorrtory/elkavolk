#include "utils.h"

QVariant readJsonProperty(const QString& filePath, const QString& propertyName) {
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly)) {
        qWarning() << "Cannot open file for reading:" << filePath;
        return QVariant();
    }
    QByteArray data = file.readAll();
    file.close();

    QJsonParseError parseError;
    QJsonDocument doc = QJsonDocument::fromJson(data, &parseError);
    if (parseError.error != QJsonParseError::NoError) {
        qWarning() << "JSON parse error:" << parseError.errorString();
        return QVariant();
    }

    return doc.object().value(propertyName).toVariant();
}

bool setJsonProperty(const QString& filePath, const QString& propertyName, const QVariant& value) {
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly)) {
        qWarning() << "Cannot open file for reading:" << filePath;
        return false;
    }
    QByteArray data = file.readAll();
    file.close();

    QJsonParseError parseError;
    QJsonDocument doc = QJsonDocument::fromJson(data, &parseError);
    if (parseError.error != QJsonParseError::NoError) {
        qWarning() << "JSON parse error:" << parseError.errorString();
        return false;
    }

    QJsonObject jsonObject = doc.object();
    jsonObject.insert(propertyName, QJsonValue::fromVariant(value));
    
    doc.setObject(jsonObject);

    if (!file.open(QIODevice::WriteOnly)) {
        qWarning() << "Cannot open file for writing:" << filePath;
        return false;
    }
    
    file.write(doc.toJson());
    file.close();
    
    return true;
}