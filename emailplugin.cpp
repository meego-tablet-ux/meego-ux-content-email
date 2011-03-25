/*
 * MeeGo Content Aggregation - Email Plugin
 * Copyright © 2010, Intel Corporation.
 */

#include <QDebug>

#include <QtPlugin>

#include "emailplugin.h"
#include "emailservicemodel.h"
#include "emailfeedmodel.h"

EmailPlugin::EmailPlugin(QObject *parent): QObject(parent), McaFeedPlugin()
{
    qDebug("EmailPlugin constructor");
    m_serviceModel = new EmailServiceModel(this);
}

EmailPlugin::~EmailPlugin()
{
}

QAbstractItemModel *EmailPlugin::serviceModel()
{
    return m_serviceModel;
}

QAbstractItemModel *EmailPlugin::createFeedModel(const QString& service)
{
    qDebug() << "EmailPlugin::createFeedModel: " << service;
    return new EmailFeedModel(m_serviceModel->accountId(service), this);
}

McaSearchableFeed *EmailPlugin::createSearchModel(const QString& service,
                                             const QString& searchText)
{
    qDebug() << "EmailPlugin::createSearchModel: " << service << searchText;
    return NULL;
}

Q_EXPORT_PLUGIN2(emailplugin, EmailPlugin)
