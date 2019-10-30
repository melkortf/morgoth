#include <QtTest/QtTest>
#include "playerinfo.h"

using namespace morgoth;

class TestPlayerInfo : public QObject {
    Q_OBJECT

private slots:
    void create();
    void setId();
    void setName();
    void setSteamId();
    void compare();

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

QTEST_MAIN(TestPlayerInfo)
#include "testplayerinfo.moc"
