/*
 * MeeGo Content Aggregation - Email Plugin
 * Copyright © 2010, Intel Corporation.
 */

#ifndef __emailplugin_h
#define __emailplugin_h

#include <QObject>

#include <feedplugin.h>

class McaServiceModel;
class McaFeedModel;
class EmailServiceModel;

class EmailPlugin: public QObject, public McaFeedPlugin
{
    Q_OBJECT
    Q_INTERFACES(McaFeedPlugin)

public:
    explicit EmailPlugin(QObject *parent = NULL);
    ~EmailPlugin();

    QAbstractItemModel *serviceModel();
    QAbstractItemModel *createFeedModel(const QString& service);
    McaSearchableFeed *createSearchModel(const QString& service,
                                         const QString& searchText);

private:
    EmailServiceModel *m_serviceModel;
};

#endif  // __emailplugin_h
