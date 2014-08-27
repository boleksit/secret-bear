#include "mainwindow.h"
#include "ui_mainwindow.h"
#define BUFSIZE 8192
#define P 0x04c11db7

/* CRC-32 const table for ASCII codes */
unsigned long int crc32_tab[256] =
{
  0x00000000, 0x77073096, 0xee0e612c, 0x990951ba,
  0x076dc419, 0x706af48f, 0xe963a535, 0x9e6495a3,
  0x0edb8832, 0x79dcb8a4, 0xe0d5e91e, 0x97d2d988,
  0x09b64c2b, 0x7eb17cbd, 0xe7b82d07, 0x90bf1d91,
  0x1db71064, 0x6ab020f2, 0xf3b97148, 0x84be41de,
  0x1adad47d, 0x6ddde4eb, 0xf4d4b551, 0x83d385c7,
  0x136c9856, 0x646ba8c0, 0xfd62f97a, 0x8a65c9ec,
  0x14015c4f, 0x63066cd9, 0xfa0f3d63, 0x8d080df5,
  0x3b6e20c8, 0x4c69105e, 0xd56041e4, 0xa2677172,
  0x3c03e4d1, 0x4b04d447, 0xd20d85fd, 0xa50ab56b,
  0x35b5a8fa, 0x42b2986c, 0xdbbbc9d6, 0xacbcf940,
  0x32d86ce3, 0x45df5c75, 0xdcd60dcf, 0xabd13d59,
  0x26d930ac, 0x51de003a, 0xc8d75180, 0xbfd06116,
  0x21b4f4b5, 0x56b3c423, 0xcfba9599, 0xb8bda50f,
  0x2802b89e, 0x5f058808, 0xc60cd9b2, 0xb10be924,
  0x2f6f7c87, 0x58684c11, 0xc1611dab, 0xb6662d3d,
  0x76dc4190, 0x01db7106, 0x98d220bc, 0xefd5102a,
  0x71b18589, 0x06b6b51f, 0x9fbfe4a5, 0xe8b8d433,
  0x7807c9a2, 0x0f00f934, 0x9609a88e, 0xe10e9818,
  0x7f6a0dbb, 0x086d3d2d, 0x91646c97, 0xe6635c01,
  0x6b6b51f4, 0x1c6c6162, 0x856530d8, 0xf262004e,
  0x6c0695ed, 0x1b01a57b, 0x8208f4c1, 0xf50fc457,
  0x65b0d9c6, 0x12b7e950, 0x8bbeb8ea, 0xfcb9887c,
  0x62dd1ddf, 0x15da2d49, 0x8cd37cf3, 0xfbd44c65,
  0x4db26158, 0x3ab551ce, 0xa3bc0074, 0xd4bb30e2,
  0x4adfa541, 0x3dd895d7, 0xa4d1c46d, 0xd3d6f4fb,
  0x4369e96a, 0x346ed9fc, 0xad678846, 0xda60b8d0,
  0x44042d73, 0x33031de5, 0xaa0a4c5f, 0xdd0d7cc9,
  0x5005713c, 0x270241aa, 0xbe0b1010, 0xc90c2086,
  0x5768b525, 0x206f85b3, 0xb966d409, 0xce61e49f,
  0x5edef90e, 0x29d9c998, 0xb0d09822, 0xc7d7a8b4,
  0x59b33d17, 0x2eb40d81, 0xb7bd5c3b, 0xc0ba6cad,
  0xedb88320, 0x9abfb3b6, 0x03b6e20c, 0x74b1d29a,
  0xead54739, 0x9dd277af, 0x04db2615, 0x73dc1683,
  0xe3630b12, 0x94643b84, 0x0d6d6a3e, 0x7a6a5aa8,
  0xe40ecf0b, 0x9309ff9d, 0x0a00ae27, 0x7d079eb1,
  0xf00f9344, 0x8708a3d2, 0x1e01f268, 0x6906c2fe,
  0xf762575d, 0x806567cb, 0x196c3671, 0x6e6b06e7,
  0xfed41b76, 0x89d32be0, 0x10da7a5a, 0x67dd4acc,
  0xf9b9df6f, 0x8ebeeff9, 0x17b7be43, 0x60b08ed5,
  0xd6d6a3e8, 0xa1d1937e, 0x38d8c2c4, 0x4fdff252,
  0xd1bb67f1, 0xa6bc5767, 0x3fb506dd, 0x48b2364b,
  0xd80d2bda, 0xaf0a1b4c, 0x36034af6, 0x41047a60,
  0xdf60efc3, 0xa867df55, 0x316e8eef, 0x4669be79,
  0xcb61b38c, 0xbc66831a, 0x256fd2a0, 0x5268e236,
  0xcc0c7795, 0xbb0b4703, 0x220216b9, 0x5505262f,
  0xc5ba3bbe, 0xb2bd0b28, 0x2bb45a92, 0x5cb36a04,
  0xc2d7ffa7, 0xb5d0cf31, 0x2cd99e8b, 0x5bdeae1d,
  0x9b64c2b0, 0xec63f226, 0x756aa39c, 0x026d930a,
  0x9c0906a9, 0xeb0e363f, 0x72076785, 0x05005713,
  0x95bf4a82, 0xe2b87a14, 0x7bb12bae, 0x0cb61b38,
  0x92d28e9b, 0xe5d5be0d, 0x7cdcefb7, 0x0bdbdf21,
  0x86d3d2d4, 0xf1d4e242, 0x68ddb3f8, 0x1fda836e,
  0x81be16cd, 0xf6b9265b, 0x6fb077e1, 0x18b74777,
  0x88085ae6, 0xff0f6a70, 0x66063bca, 0x11010b5c,
  0x8f659eff, 0xf862ae69, 0x616bffd3, 0x166ccf45,
  0xa00ae278, 0xd70dd2ee, 0x4e048354, 0x3903b3c2,
  0xa7672661, 0xd06016f7, 0x4969474d, 0x3e6e77db,
  0xaed16a4a, 0xd9d65adc, 0x40df0b66, 0x37d83bf0,
  0xa9bcae53, 0xdebb9ec5, 0x47b2cf7f, 0x30b5ffe9,
  0xbdbdf21c, 0xcabac28a, 0x53b39330, 0x24b4a3a6,
  0xbad03605, 0xcdd70693, 0x54de5729, 0x23d967bf,
  0xb3667a2e, 0xc4614ab8, 0x5d681b02, 0x2a6f2b94,
  0xb40bbe37, 0xc30c8ea1, 0x5a05df1b, 0x2d02ef8d
};

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    manager = new QNetworkAccessManager(this);
    QNetworkProxy proxy(QNetworkProxy::HttpCachingProxy, "127.0.0.1", 8888);
    manager->setProxy(proxy);

    connect(ui->inputButton1, SIGNAL(clicked()), this, SLOT(chooseInput1()));
    connect(ui->inputButton2, SIGNAL(clicked()), this, SLOT(chooseInput2()));
    connect(ui->inputButton3, SIGNAL(clicked()), this, SLOT(chooseInput3()));
    connect(ui->inputButton4, SIGNAL(clicked()), this, SLOT(chooseInput4()));
    connect(ui->inputButton5, SIGNAL(clicked()), this, SLOT(chooseInput5()));
    connect(ui->outputButton1, SIGNAL(clicked()), this, SLOT(chooseOutput1()));
    connect(ui->outputButton2, SIGNAL(clicked()), this, SLOT(chooseOutput2()));
    connect(ui->outputButton6, SIGNAL(clicked()), this, SLOT(chooseOutput6()));
    connect(ui->startButton1, SIGNAL(clicked()), this, SLOT(encode()));
    connect(ui->startButton2, SIGNAL(clicked()), this, SLOT(decode()));
    connect(ui->startButton3, SIGNAL(clicked()), this, SLOT(compare()));
    connect(ui->startButton5, SIGNAL(clicked()), this, SLOT(upload()));
    connect(ui->startButton6, SIGNAL(clicked()), this, SLOT(download()));
    connect(ui->encodeSlider, SIGNAL(sliderMoved(int)), ui->encodeSliderLabel, SLOT(setNum(int)));
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::encode() {
    QTime time = QTime::currentTime();
    ui->doneLabel1->setText("PROCESSING...");
    int dziel = ui->encodeSlider->value() * 1024 * 1024, p = 0;
    QFile input(ui->inputLineEdit1->text());
    if(!input.open(QIODevice::ReadOnly)) {
        ui->doneLabel1->setText("ERROR");
        return;
    }
    int size;
    do {
        QByteArray blob;
        if(dziel == 0)
            blob = input.readAll();
        else
            blob = input.read(dziel);
        size = blob.size();

        QImage image(1024, (size+1)/4/1024 + 1, QImage::Format_ARGB32);
        image.fill(Qt::transparent);

        QColor size_p;
        int asize = size / (256*256*256);
        if(asize > 255) {
            ui->doneLabel1->setText("ERROR");
            return;
        }
        int bsize = (size - (asize * 256 * 256 * 256)) / (256*256);
        int gsize = (size - (bsize * 256 * 256) - (asize * 256 * 256 * 256)) / 256;
        int rsize = size - (gsize * 256) - (bsize * 256 * 256) - (asize * 256 * 256 * 256);
        size_p.setRgb(rsize, gsize, bsize, asize);
        image.setPixel(0, 0, size_p.rgba());

        int i = 0, j = 0;
        size++;
        for(int k = 0; k < size; i++, j++, k += 4) {
            if(i == 0 && j == 0) {
                i++;
                j++;
            }
            if(j > 1023) j = 0;
            int r = blob.at(k) + 128, g = 0, b = 0, a = 0;
            if(k + 1 < size)
                g = blob.at(k + 1) + 128;
            if(k + 2 < size)
                b = blob.at(k + 2) + 128;
            if(k + 3 < size)
                a = blob.at(k + 3) + 128;
            QColor c(r, g, b, a);
            image.setPixel(j, i/1024, c.rgba());
        }
        if(dziel == 0 || p == 0) {
            p++;
            image.save(ui->outputLineEdit1->text());
        } else
            image.save(ui->outputLineEdit1->text().replace(".", QString::number(p++) + "."));
    } while(size - 1 == dziel);

    input.close();
    ui->doneLabel1->setText(QString::number(time.msecsTo(QTime::currentTime())));
    return;
}

void MainWindow::decode() {
    QTime time = QTime::currentTime();
    ui->doneLabel2->setText("PROCESSING...");
    QImage image(1, 1, QImage::Format_ARGB32);
    QFile output(ui->outputLineEdit2->text());
    if(!output.open(QIODevice::WriteOnly) || !image.load(ui->inputLineEdit2->text())) {
        ui->doneLabel2->setText("ERROR");
        return;
    }

    int p = 1;
    while(true) {
        QByteArray out;

        QRgb rgb = image.pixel(0,0);
        int size = qRed(rgb) + qGreen(rgb)*256 + qBlue(rgb)*256*256 + qAlpha(rgb)*256*256*256 + 1;

        int k = 1;
        for(int j = 0; j < image.height(); j++) {
            for(int i = 0; i < image.width(); i++) {
                if(i == 0 && j == 0)
                    continue;
                rgb = image.pixel(i, j);
                if (++k > size) break;
                out.append(char(qRed(rgb) - 128));
                if (++k > size) break;
                out.append(char(qGreen(rgb) - 128));
                if (++k > size) break;
                out.append(char(qBlue(rgb) - 128));
                if (++k > size) break;
                out.append(char(qAlpha(rgb) - 128));
            }
        }

        output.write(out);
        if(!ui->checkBox->isChecked())
            break;
        if(!image.load(ui->inputLineEdit2->text().replace(".", QString::number(p++) + ".")))
            break;
    }
    output.close();
    ui->doneLabel2->setText(QString::number(time.msecsTo(QTime::currentTime())));
    return;
}

void MainWindow::compare() {
    QTime time = QTime::currentTime();
    ui->doneLabel3->setText("PROCESSING...");
    QImage image(1, 1, QImage::Format_ARGB32);
    QFile input(ui->inputLineEdit3->text());
    if(!input.open(QIODevice::ReadOnly) || !image.load(ui->inputLineEdit4->text())) {
        ui->doneLabel2->setText("ERROR");
        return;
    }

    QByteArray in = input.readAll();
    int size = in.size();

    int k = 1;
    for(int j = 0; j < image.height(); j++) {
        for(int i = 0; i < image.width(); i++) {
            if(i == 0 && j == 0)
                continue;
            QRgb rgb = image.pixel(i, j);
            if (++k > size) break;
            if(char(qRed(rgb) - 128) != in.at(k - 2)) {
                ui->doneLabel3->setText(QString::number(k - 2) + " " + QString::number(qRed(rgb) - 128) + " " + QString::number(in.at(k - 2)));
                return;
            }
            if (++k > size) break;
            if(char(qGreen(rgb) - 128) != in.at(k - 2)) {
                ui->doneLabel3->setText(QString::number(k - 2) + " " + QString::number(qGreen(rgb) - 128) + " " + QString::number(in.at(k - 2)));
                return;
            }
            if (++k > size) break;
            if(char(qBlue(rgb) - 128) != in.at(k - 2)) {
                ui->doneLabel3->setText(QString::number(k - 2) + " " + QString::number(qBlue(rgb) - 128) + " " + QString::number(in.at(k - 2)));
                return;
            }
            if (++k > size) break;
            if(char(qAlpha(rgb) - 128) != in.at(k - 2)) {
                ui->doneLabel3->setText(QString::number(k - 2) + " " + QString::number(qAlpha(rgb) - 128) + " " + QString::number(in.at(k - 2)));
                return;
            }
        }
    }
    input.close();
    ui->doneLabel3->setText(QString::number(time.msecsTo(QTime::currentTime())));
    return;
}

void MainWindow::upload() {
    QTime time = QTime::currentTime();
    ui->doneLabel5->setText("PROCESSING...");
    ui->outputLineEdit5->setText("");
    QFile file(ui->inputLineEdit5->text());
    if(!file.open(QIODevice::ReadOnly)) {
        ui->doneLabel5->setText("ERROR");
        return;
    }
    QString url, content, contentType = "image/png";
    QList<QHttpPart> additionalParts;
    QNetworkRequest req;
    QHttpPart part;
    switch(ui->comboBox->currentIndex()) {
        case 0:
            url = "http://abload.de/upload.php";
            content = "form-data; name=\"img0\"; filename=\"output.png\"";
            part.setHeader(QNetworkRequest::ContentDispositionHeader, "form-data; name=\"resize\"");
            part.setBody("none");
            additionalParts.append(part);
            part.setHeader(QNetworkRequest::ContentDispositionHeader, "form-data; name=\"delete\"");
            part.setBody("never");
            additionalParts.append(part);
            break;
        case 1:
            url = "http://fastpic.ru/uploadmulti";
            content = "form-data; name=\"file[]\"; filename=\"output.png\"";
            part.setHeader(QNetworkRequest::ContentDispositionHeader, "form-data; name=\"check_thumb\"");
            part.setBody("size");
            additionalParts.append(part);
            part.setHeader(QNetworkRequest::ContentDispositionHeader, "form-data; name=\"thumb_text\"");
            part.setBody("Увеличить");
            additionalParts.append(part);
            part.setHeader(QNetworkRequest::ContentDispositionHeader, "form-data; name=\"thumb_size\"");
            part.setBody("170");
            additionalParts.append(part);
            part.setHeader(QNetworkRequest::ContentDispositionHeader, "form-data; name=\"res_select\"");
            part.setBody("500");
            additionalParts.append(part);
            part.setHeader(QNetworkRequest::ContentDispositionHeader, "form-data; name=\"orig_resize\"");
            part.setBody("500");
            additionalParts.append(part);
            part.setHeader(QNetworkRequest::ContentDispositionHeader, "form-data; name=\"orig_rotate\"");
            part.setBody("0");
            additionalParts.append(part);
            part.setHeader(QNetworkRequest::ContentDispositionHeader, "form-data; name=\"jpeg_quality\"");
            part.setBody("75");
            additionalParts.append(part);
            part.setHeader(QNetworkRequest::ContentDispositionHeader, "form-data; name=\"submit\"");
            part.setBody("Загрузить");
            additionalParts.append(part);
            part.setHeader(QNetworkRequest::ContentDispositionHeader, "form-data; name=\"uploading\"");
            part.setBody("1");
            additionalParts.append(part);
            break;
        case 2:
            url = "http://minus.com/api/UploadItem_Singleton/";
            content = "form-data; name=\"file\"; filename=\"output.png\"";
            part.setHeader(QNetworkRequest::ContentDispositionHeader, "form-data; name=\"filename\"");
            part.setBody("output.png");
            additionalParts.append(part);
            part.setHeader(QNetworkRequest::ContentDispositionHeader, "form-data; name=\"caption\"");
            part.setBody("");
            additionalParts.append(part);
            break;
    }

    req.setUrl(QUrl(url));
    QHttpMultiPart *multi = new QHttpMultiPart;
    QHttpPart partImg;

    partImg.setBody(file.readAll());
    partImg.setHeader(QNetworkRequest::ContentDispositionHeader, content);
    partImg.setHeader(QNetworkRequest::ContentTypeHeader, contentType);
    multi->append(partImg);

    for(int i = 0; i < additionalParts.size(); i++)
        multi->append(additionalParts.at(i));

    req.setHeader(QNetworkRequest::ContentTypeHeader, "multipart/form-data; boundary=" + QString(multi->boundary()));

    connect(manager, SIGNAL(finished(QNetworkReply*)), this, SLOT(uploaded(QNetworkReply*)));
    multi->setContentType(QHttpMultiPart::FormDataType);
    QNetworkReply *reply = manager->post(req, multi);
    connect(reply, SIGNAL(uploadProgress(qint64,qint64)), this, SLOT(uploadProgress(qint64, qint64)));

    ui->doneLabel5->setText(QString::number(time.msecsTo(QTime::currentTime())));

    return;
}

void MainWindow::uploaded(QNetworkReply *reply) {
    QString s = reply->readAll();
    QString loc = reply->rawHeader(QByteArray("Location"));
    QString ref = reply->rawHeader(QByteArray("Refresh"));
    if(s.contains("s:15:")) {
        s.remove(0, s.indexOf("s:15:") + 11);
        s.remove(s.indexOf("&"), s.length());
        s = "http://abload.de/img/" + s;
        ui->outputLineEdit5->setText(s);
    } else if(s.contains("{\"gallery_id\":")) {
        s.remove(0, s.indexOf("\"id\":") + 7);
        s.remove(s.indexOf("\","), s.length());
        s = "http://i.minus.com/i" + s + ".png";
        QNetworkRequest req;
        req.setUrl(QUrl(s));
        manager->get(req);
        return;
    } else if(loc.contains("minus.com")) {
        ui->outputLineEdit5->setText(loc);
    } else if(ref.contains("fastpic.ru")) {
        ref.remove(0, ref.indexOf("http"));
        QNetworkRequest req;
        req.setUrl(QUrl(ref));
        manager->get(req);
        return;
    } else if(s.contains("<title>FastPic")) {
        s.remove(0, s.indexOf("size=\"85\"") + 17);
        s.remove(s.indexOf("\">"), s.length());
        ui->outputLineEdit5->setText(s);
    } else {
        QList<QNetworkReply::RawHeaderPair> list = reply->rawHeaderPairs();
        QString lol = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toString();
        lol.append(":\n");
        for(int i = 0; i < list.count(); i++) {
            lol.append(list.at(i).first + ": " +  list.at(i).second + "\n");
        }
        lol.append("\n" + s);
        ui->textEdit->setText(lol);
    }
    ui->progressBar1->setValue(0);
    disconnect(manager, SIGNAL(finished(QNetworkReply*)), this, SLOT(uploaded(QNetworkReply*)));
    return;
}

void MainWindow::uploadProgress(qint64 sent, qint64 total) {
    int wynik;
    if(sent == total)
        wynik = 100;
    else
        wynik = sent*100/total;
    ui->progressBar1->setValue(wynik);
    ui->progressBar1->repaint();
    return;
}

void MainWindow::download() {
    QTime time = QTime::currentTime();
    ui->doneLabel6->setText("PROCESSING...");

    QNetworkRequest req(QUrl(ui->inputLineEdit6->text()));
    QNetworkReply *reply = manager->get(req);
    connect(manager, SIGNAL(finished(QNetworkReply*)), this, SLOT(downloaded(QNetworkReply*)));
    connect(reply, SIGNAL(downloadProgress(qint64,qint64)), this, SLOT(downloadProgress(qint64,qint64)));

    ui->doneLabel6->setText(QString::number(time.msecsTo(QTime::currentTime())));
    return;
}

void MainWindow::downloaded(QNetworkReply *reply) {
    disconnect(manager, SIGNAL(finished(QNetworkReply*)), this, SLOT(downloaded(QNetworkReply*)));
    QFile output(ui->outputLineEdit6->text());
    if(!output.open(QIODevice::WriteOnly)) {
        ui->doneLabel6->setText("ERROR");
        return;
    }
    output.write(reply->readAll());
    output.close();
    return;
}

void MainWindow::downloadProgress(qint64 recieved, qint64 total) {
    int wynik;
    if(recieved == total)
        wynik = 100;
    else
        wynik = recieved*100/total;
    ui->progressBar2->setValue(wynik);
    ui->progressBar2->repaint();
    return;
}

void MainWindow::hashCreate()
{
    QString hash;


    hash+="P2ID:";
    hash+=ui->inputLineEdit1->text().section('/',-1);
    hash+=crcCalc("")
    hash+="[";

    for(int i=1; i<=ile_segow; i++)
    {
        hash+=i;
        for(int j=0; j<ile_hostow; j++)
            hash+='/'+tab[i][j]+'/';
    }

    hash+=']';
}

quint16 MainWindow::crcCalc(QString path)
{
    QFile file(path);
    QByteArray blob = file.readAll();
    quint16 crc = qChecksum(blob, file.size());
    return crc;
}

void MainWindow::chooseInput1() {
    ui->inputLineEdit1->setText(QFileDialog::getOpenFileName(NULL, "Open File", QDir::currentPath(),  "Files (*.*)"));
    return;
}

void MainWindow::chooseInput2() {
    ui->inputLineEdit2->setText(QFileDialog::getOpenFileName(NULL, "Open File", QDir::currentPath(),  "Images (*.PNG)"));
    return;
}

void MainWindow::chooseInput3() {
    ui->inputLineEdit3->setText(QFileDialog::getOpenFileName(NULL, "Open File", QDir::currentPath(),  "Files (*.*)"));
    return;
}

void MainWindow::chooseInput4() {
    ui->inputLineEdit4->setText(QFileDialog::getOpenFileName(NULL, "Open File", QDir::currentPath(),  "Images (*.PNG)"));
    return;
}

void MainWindow::chooseInput5() {
    ui->inputLineEdit5->setText(QFileDialog::getOpenFileName(NULL, "Open File", QDir::currentPath(),  "Images (*.PNG)"));
    return;
}

void MainWindow::chooseOutput1() {
    ui->outputLineEdit1->setText(QFileDialog::getSaveFileName(NULL, "Save File", QDir::currentPath(),  "Images (*.PNG)"));
    return;
}

void MainWindow::chooseOutput2() {
    ui->outputLineEdit2->setText(QFileDialog::getSaveFileName(NULL, "Save File", QDir::currentPath(),  "Files (*.*)"));
    return;
}

void MainWindow::chooseOutput6() {
    ui->outputLineEdit6->setText(QFileDialog::getSaveFileName(NULL, "Save File", QDir::currentPath(),  "Images (*.PNG)"));
    return;
}
