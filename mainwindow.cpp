#include "mainwindow.h"
#include "ui_mainwindow.h"

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
