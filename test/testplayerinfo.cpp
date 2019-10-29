#include <QtTest/QtTest>
#include "playerinfo.h"

using namespace morgoth;

class TestPlayerInfo : public QObject {
    Q_OBJECT

private:
    void create();
    void setId();
    void setName();
    void setSteamId();
    void compare();
    void dbusMarshall();

};

void TestPlayerInfo::create()
{
    PlayerInfo playerInfo(5);
    QCOMPARE(playerInfo.id(), 5);
}

void TestPlayerInfo::setId()
{
    PlayerInfo playerInfo;
    playerInfo.setId(25);
    QCOMPARE(playerInfo.id(), 25);
}

void TestPlayerInfo::setName()
{
    PlayerInfo playerInfo;
    playerInfo.setName("foo bar");
    QCOMPARE(playerInfo.name(), "foo bar");
}

void TestPlayerInfo::setSteamId()
{
    PlayerInfo playerInfo;
    playerInfo.setSteamId(SteamId(Q_UINT64_C(76561198074409147)));
    QCOMPARE(playerInfo.steamId(), SteamId(Q_UINT64_C(76561198074409147)));
}

void TestPlayerInfo::compare()
{
    PlayerInfo a(35);
    a.setName("fake name");
    a.setSteamId(SteamId(Q_UINT64_C(76561198074409147)));

    PlayerInfo b;
    b.setId(35);
    b.setName("fake name");
    b.setSteamId(SteamId(Q_UINT64_C(76561198074409147)));

    QCOMPARE(a, b);
}

void TestPlayerInfo::dbusMarshall()
{
    {
        PlayerInfo a(123);
        a.setName("fake name");
        a.setSteamId(SteamId(Q_UINT64_C(76561198074409147)));

        QDBusArgument dbusArgument;
        dbusArgument << a;
        QCOMPARE(dbusArgument.currentType(), QDBusArgument::ElementType::UnknownType);

        PlayerInfo b;
        dbusArgument >> b;
        QCOMPARE(a, b);
    }

    {
        PlayerInfo a(998);
        a.setName("fake name 1");
        a.setSteamId(SteamId(Q_UINT64_C(1)));

        PlayerInfo b(999);
        b.setName("fake name 2");
        b.setSteamId(SteamId(Q_UINT64_C(2)));

        PlayerInfoList listA({ a, b });
        QDBusArgument dbusArgument;
        dbusArgument << listA;
        QCOMPARE(dbusArgument.currentType(), QDBusArgument::ElementType::UnknownType);

        PlayerInfoList listB;
        dbusArgument >> listB;
        QCOMPARE(listA, listB);
    }
}

QTEST_MAIN(TestPlayerInfo)
#include "testplayerinfo.moc"
