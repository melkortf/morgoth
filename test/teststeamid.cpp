#include <QtTest/QtTest>
#include "steamid.h"

using morgoth::SteamId;

class TestSteamId : public QObject {
    Q_OBJECT

private slots:
    void testFromCommunityId();
    void testFromText();
    void testFromId64();
    void testToId64();
    void testToText();
    void testToCommunityId();

};

constexpr auto MalySteamId = Q_UINT64_C(76561198074409147);

void TestSteamId::testFromCommunityId()
{
    SteamId steamId("[U:1:114143419]", SteamId::Format::Id3);
    QCOMPARE(steamId.toSteamId64(), MalySteamId);

    // no square-brackets should be accepted as well
    SteamId steamId2("U:1:114143419", SteamId::Format::Id3);
    QCOMPARE(steamId2.toSteamId64(), MalySteamId);
}

void TestSteamId::testFromText()
{
    SteamId steamId("STEAM_0:1:57071709", SteamId::Format::Text);
    QCOMPARE(steamId.toSteamId64(), MalySteamId);
}

void TestSteamId::testFromId64()
{
    SteamId steamId("76561198074409147", SteamId::Format::Id64);
    QCOMPARE(steamId.toSteamId64(), MalySteamId);
}

void TestSteamId::testToId64()
{
    SteamId steamId(MalySteamId);
    QCOMPARE(steamId.toFormat(SteamId::Format::Id64), "76561198074409147");
}

void TestSteamId::testToText()
{
    SteamId steamId(MalySteamId);
    QCOMPARE(steamId.toFormat(SteamId::Format::Text), "STEAM_0:1:57071709");
}

void TestSteamId::testToCommunityId()
{
    SteamId steamId(MalySteamId);
    QCOMPARE(steamId.toFormat(SteamId::Format::Id3), "[U:1:114143419]");
}

QTEST_MAIN(TestSteamId)
#include "teststeamid.moc"
