#include <QtCore/QString>
#include <QtTest/QtTest>

class Vector : public QObject
{
    Q_OBJECT
    
public:
    Vector();
    
private Q_SLOTS:
    void initTestCase();
    void cleanupTestCase();
    void Operators();
    void Operators_data();
};

Vector::Vector()
{
}

void Vector::initTestCase()
{
}

void Vector::cleanupTestCase()
{
}

void Vector::Operators()
{
    QFETCH(QString, data);
    QVERIFY2(true, "Failure");
}

void Vector::Operators_data()
{
    QTest::addColumn<QString>("data");
    QTest::newRow("0") << QString();
}

QTEST_APPLESS_MAIN(Vector)

#include "tst_vector.moc"
