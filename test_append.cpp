#include <QFile>
#include <QTextStream>
#include <QDebug>

int main() {
    // Create a test file with some content
    {
        QFile f("test_append.txt");
        f.open(QIODevice::WriteOnly | QIODevice::Text);
        QTextStream s(&f);
        s << "Initial content\n";
        s << "Second line\n";
        f.close();
    }
    
    // Now open in append mode and check pos() vs size()
    QFile f("test_append.txt");
    f.open(QIODevice::WriteOnly | QIODevice::Append | QIODevice::Text);
    QTextStream s(&f);
    
    qDebug() << "After opening in Append mode:";
    qDebug() << "  pos():" << f.pos();
    qDebug() << "  size():" << f.size();
    
    // Write something
    s << "New line\n";
    s.flush();
    
    qDebug() << "After writing one line:";
    qDebug() << "  pos():" << f.pos();
    qDebug() << "  size():" << f.size();
    
    f.close();
    QFile::remove("test_append.txt");
    return 0;
}
