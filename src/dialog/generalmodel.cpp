#include "generalmodel.h"
#include "engine.h"
#include "general.h"
#include "skill.h"

#include <QFile>

GeneralCompleterModel::GeneralCompleterModel()
{
    foreach(const General *g, Sanguosha->findChildren<const General *>()){
        if(!g->isTotallyHidden())
            list << g;
    }

    foreach(const QObject *obj, list){
        const General *g = qobject_cast<const General *>(obj);
        foreach(const Skill *s, g->getVisibleSkills())
            list << s;
    }
}

int GeneralCompleterModel::rowCount(const QModelIndex &parent) const
{
    return parent.isValid() ? 0 : list.length();
}

QVariant GeneralCompleterModel::data(const QModelIndex &index, int role) const
{
    const QObject *obj = static_cast<const QObject *>(index.internalPointer());

    if(obj->inherits("General")){
        const General *g = qobject_cast<const General *>(obj);

        switch(role){
        case Qt::EditRole: return g->objectName();
        case Qt::DisplayRole: return QString("%1 (%2)").arg(Sanguosha->translate(g->objectName())).arg(g->objectName());
        case Qt::BackgroundRole: if(g->isHidden()) return QBrush(Qt::gray);
        case Qt::DecorationRole: {
            QString path(g->getPixmapPath("tiny"));
            if(QFile::exists(path)){
                return QIcon(path);
            }else{
                return QIcon("image/system/tiny_unknown.png");
            }
        }
        }
    }else if(obj->inherits("Skill")){
        const Skill *s = qobject_cast<const Skill *>(obj);
        switch(role){
        case Qt::EditRole: return s->objectName();
        case Qt::DisplayRole: {
            const General *g = qobject_cast<const General *>(s->parent());
            if(g){
                return tr("%1 has skill %2 (%3)")
                        .arg(Sanguosha->translate(g->objectName()))
                        .arg(Sanguosha->translate(s->objectName()))
                        .arg(s->objectName());
            }
        }
        case Qt::DecorationRole:{
            const General *g = qobject_cast<const General *>(s->parent());
            if(g){
                return QIcon(g->getPixmapPath("tiny"));
            }
        }
        }
    }

    return QVariant();
}

QModelIndex GeneralCompleterModel::index(int row, int column, const QModelIndex &parent) const
{
    if(parent.isValid() || column != 0 || row < 0 || row >= list.length())
        return QModelIndex();
    else
        return createIndex(row, column, const_cast<QObject *>(list[row]));
}

static bool CompareByName(const General *g1, const General *g2){
    return g1->objectName() < g2->objectName();
}

void GeneralListModel::doSearch(const GeneralListModel::SearchOptions &options)
{
    beginResetModel();

    GeneralList list;
    QHashIterator<QString, const General *> itor(Sanguosha->getGenerals());
    while(itor.hasNext()){
        const General *g = itor.next().value();

        if(g->isTotallyHidden())
            continue;

        if(!options.kingdom.isEmpty() && g->getKingdom() != options.kingdom)
            continue;

        if(!options.package.isEmpty() && g->getPackage() != options.package)
            continue;

        if(!options.gender.isEmpty() && g->getGenderString() != options.gender)
            continue;

        list << g;
    }

    qSort(list.begin(), list.end(), CompareByName);

    this->list = list;

    endResetModel();
}


int GeneralListModel::rowCount(const QModelIndex &parent) const
{
    return parent.isValid() ? 0 : list.length();
}

QVariant GeneralListModel::data(const QModelIndex &index, int role) const
{
    if(index.row() < 0 || index.row() >= list.length())
        return QVariant();

    const General *g = list[index.row()];
    switch(role){
    case Qt::DisplayRole: return Sanguosha->translate(g->objectName());
    case Qt::DecorationRole: {
        QString path(g->getPixmapPath("tiny"));
        if(QFile::exists(path)){
            return QIcon(path);
        }else{
            return QIcon("image/system/tiny_unknown.png");
        }
    }
    case Qt::ForegroundRole: {
        if(g->isLord())
            return QBrush(Qt::red);
        else
            return QVariant();
    }
    case Qt::BackgroundRole: if(g->isHidden()) return QBrush(Qt::gray);
    }

    return QVariant();
}

QModelIndex GeneralListModel::index(int row, int column, const QModelIndex &parent) const
{
    if(parent.isValid() || column || row < 0 || row >= list.length())
        return QModelIndex();
    else
        return createIndex(row, column, (void *)(list[row]));
}

GeneralListModel::GeneralListModel()
{
}