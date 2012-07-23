#include "ValidateViewWidget.h"

#include <QtGui/QGraphicsRectItem>
#include <QtGui/QGraphicsSimpleTextItem>

#include <validator/validator-config.h>
#include <validator/validator.h>

#include <qdebug.h>

#define RES_GET16(s, cp) do { \
        register const u_char *t_cp = (const u_char *)(cp); \
        (s) = ((u_int16_t)t_cp[0] << 8) \
            | ((u_int16_t)t_cp[1]) \
            ; \
        (cp) += NS_INT16SZ; \
} while (0)

// from ns_print.c
extern "C" {
u_int16_t id_calc(const u_char * key, const int keysize);
}

ValidateViewWidget::ValidateViewWidget(QString nodeName, QString recordType, QWidget *parent) :
    QGraphicsView(parent), m_nodeName(nodeName), m_recordType(recordType), m_typeToName(), m_statusToName()
{
    myScene = new QGraphicsScene(this);
    myScene->setItemIndexMethod(QGraphicsScene::NoIndex);
    myScene->setSceneRect(0, 0, 600, 600);
    setScene(myScene);
    setCacheMode(CacheBackground);
    setViewportUpdateMode(BoundingRectViewportUpdate);
    setRenderHint(QPainter::Antialiasing);
    setTransformationAnchor(AnchorUnderMouse);
    setDragMode(QGraphicsView::ScrollHandDrag);
    setWindowTitle(tr("Validation of %1 for %2").arg(nodeName).arg(recordType));
    //scaleWindow();

    m_typeToName[1] = "A";
    m_typeToName[2] = "NS";
    m_typeToName[5] = "CNAME";
    m_typeToName[6] = "SOA";
    m_typeToName[12] = "PTR";
    m_typeToName[15] = "MX";
    m_typeToName[16] = "TXT";
    m_typeToName[28] = "AAAA";
    m_typeToName[33] = "SRV";
    m_typeToName[255] = "ANY";

    m_typeToName[43] = "DS";
    m_typeToName[46] = "RRSIG";
    m_typeToName[47] = "NSEC";
    m_typeToName[48] = "DNSKEY";
    m_typeToName[50] = "NSEC3";
    m_typeToName[32769] = "DLV";

    m_typeToName[3] = "MD";
    m_typeToName[4] = "MF";
    m_typeToName[7] = "MB";
    m_typeToName[8] = "MG";
    m_typeToName[9] = "MR";
    m_typeToName[10] = "NULL";
    m_typeToName[11] = "WKS";
    m_typeToName[13] = "HINFO";
    m_typeToName[14] = "MINFO";
    m_typeToName[17] = "RP";
    m_typeToName[18] = "AFSDB";
    m_typeToName[19] = "X25";
    m_typeToName[20] = "ISDN";
    m_typeToName[21] = "RT";
    m_typeToName[22] = "NSAP";
    m_typeToName[23] = "NSAP_PTR";
    m_typeToName[24] = "SIG";
    m_typeToName[25] = "KEY";
    m_typeToName[26] = "PX";
    m_typeToName[27] = "GPOS";
    m_typeToName[29] = "LOC";
    m_typeToName[30] = "NXT";
    m_typeToName[31] = "EID";
    m_typeToName[32] = "NIMLOC";
    m_typeToName[34] = "ATMA";
    m_typeToName[35] = "NAPTR";
    m_typeToName[36] = "KX";
    m_typeToName[37] = "CERT";
    m_typeToName[38] = "A6";
    m_typeToName[39] = "DNAME";
    m_typeToName[40] = "SINK";
    m_typeToName[41] = "OPT";
    m_typeToName[250] = "TSIG";
    m_typeToName[251] = "IXFR";
    m_typeToName[252] = "AXFR";
    m_typeToName[253] = "MAILB";
    m_typeToName[254] = "MAILA";

    m_statusToName[VAL_AC_UNSET] = "UNSET";
    m_statusToName[VAL_AC_CAN_VERIFY] = "CAN_VERIFY";
    m_statusToName[VAL_AC_WAIT_FOR_TRUST] = "WAIT_FOR_TRUST";
    m_statusToName[VAL_AC_WAIT_FOR_RRSIG] = "WAIT_FOR_RRSIG";
    m_statusToName[VAL_AC_TRUST_NOCHK] = "TRUST_NOCHK";
    m_statusToName[VAL_AC_INIT] = "INIT";
    m_statusToName[VAL_AC_NEGATIVE_PROOF] = "NEGATIVE_PROOF";
    m_statusToName[VAL_AC_DONT_GO_FURTHER] = "DONT_GO_FURTHER";
    m_statusToName[VAL_AC_IGNORE_VALIDATION] = "IGNORE_VALIDATION";
    m_statusToName[VAL_AC_UNTRUSTED_ZONE] = "UNTRUSTED_ZONE";
    m_statusToName[VAL_AC_PINSECURE] = "PINSECURE";
    m_statusToName[VAL_AC_BARE_RRSIG] = "BARE_RRSIG";
    m_statusToName[VAL_AC_NO_LINK] = "NO_LINK";
    m_statusToName[VAL_AC_TRUST_ANCHOR] = "TRUST_ANCHOR";
    m_statusToName[VAL_AC_TRUST] = "TRUST";
    m_statusToName[VAL_AC_LAST_STATE] = "LAST_STATE";
    m_statusToName[VAL_AC_ERROR_BASE] = "ERROR_BASE";
    m_statusToName[VAL_AC_RRSIG_MISSING] = "RRSIG_MISSING";
    m_statusToName[VAL_AC_DNSKEY_MISSING] = "DNSKEY_MISSING";
    m_statusToName[VAL_AC_DS_MISSING] = "DS_MISSING";
    m_statusToName[VAL_AC_LAST_ERROR] = "LAST_ERROR";
    m_statusToName[VAL_AC_BAD_BASE] = "BAD_BASE";
    m_statusToName[VAL_AC_DATA_MISSING] = "DATA_MISSING";
    m_statusToName[VAL_AC_DNS_ERROR] = "DNS_ERROR";
    m_statusToName[VAL_AC_LAST_BAD] = "LAST_BAD";
    m_statusToName[VAL_AC_FAIL_BASE] = "FAIL_BASE";
    m_statusToName[VAL_AC_NOT_VERIFIED] = "NOT_VERIFIED";
    m_statusToName[VAL_AC_WRONG_LABEL_COUNT] = "WRONG_LABEL_COUNT";
    m_statusToName[VAL_AC_INVALID_RRSIG] = "INVALID_RRSIG";
    m_statusToName[VAL_AC_RRSIG_NOTYETACTIVE] = "RRSIG_NOTYETACTIVE";
    m_statusToName[VAL_AC_RRSIG_EXPIRED] = "RRSIG_EXPIRED";
    m_statusToName[VAL_AC_RRSIG_VERIFY_FAILED] = "RRSIG_VERIFY_FAILED";
    m_statusToName[VAL_AC_RRSIG_ALGORITHM_MISMATCH] = "RRSIG_ALGORITHM_MISMATCH";
    m_statusToName[VAL_AC_DNSKEY_NOMATCH] = "DNSKEY_NOMATCH";
    m_statusToName[VAL_AC_UNKNOWN_DNSKEY_PROTOCOL] = "UNKNOWN_DNSKEY_PROTOCOL";
    m_statusToName[VAL_AC_DS_NOMATCH] = "DS_NOMATCH";
    m_statusToName[VAL_AC_INVALID_KEY] = "INVALID_KEY";
    m_statusToName[VAL_AC_INVALID_DS] = "INVALID_DS";
    m_statusToName[VAL_AC_ALGORITHM_NOT_SUPPORTED] = "ALGORITHM_NOT_SUPPORTED";
    m_statusToName[VAL_AC_LAST_FAILURE] = "LAST_FAILURE";
    m_statusToName[VAL_AC_VERIFIED] = "VERIFIED";
    m_statusToName[VAL_AC_RRSIG_VERIFIED] = "RRSIG_VERIFIED";
    m_statusToName[VAL_AC_WCARD_VERIFIED] = "WCARD_VERIFIED";
    m_statusToName[VAL_AC_RRSIG_VERIFIED_SKEW] = "RRSIG_VERIFIED_SKEW";
    m_statusToName[VAL_AC_WCARD_VERIFIED_SKEW] = "WCARD_VERIFIED_SKEW";
    m_statusToName[VAL_AC_TRUST_POINT] = "TRUST_POINT";
    m_statusToName[VAL_AC_SIGNING_KEY] = "SIGNING_KEY";
    m_statusToName[VAL_AC_VERIFIED_LINK] = "VERIFIED_LINK";
    m_statusToName[VAL_AC_UNKNOWN_ALGORITHM_LINK] = "UNKNOWN_ALGORITHM_LINK";

    scaleView(.5);
    validateSomething(m_nodeName, m_recordType);
}

void ValidateViewWidget::scaleView(qreal scaleFactor)
{
    qreal factor = transform().scale(scaleFactor, scaleFactor).mapRect(QRectF(0, 0, 1, 1)).width();
    if (factor < 0.07 || factor > 100)
        return;

    scale(scaleFactor, scaleFactor);
}

void ValidateViewWidget::drawArrow(int fromX, int fromY, int toX, int toY) {
    const int arrowHalfWidth = 10;

    QGraphicsLineItem *line = new QGraphicsLineItem(fromX, fromY, toX, toY);
    myScene->addItem(line);

    QPolygon polygon;
    polygon << QPoint(toX, toY)
            << QPoint(toX - arrowHalfWidth, toY - arrowHalfWidth)
            << QPoint(toX + arrowHalfWidth, toY - arrowHalfWidth);
    QGraphicsPolygonItem *polyItem = new QGraphicsPolygonItem(polygon);
    polyItem->setBrush(QBrush(Qt::black));
    polyItem->setFillRule(Qt::OddEvenFill);
    myScene->addItem(polyItem);
}

void ValidateViewWidget::validateSomething(QString name, QString type) {
    val_result_chain                *results = 0;
    struct val_authentication_chain *vrcptr = 0;

    const int boxWidth = 400;
    const int boxHeight = 120;
    const int spacing = boxHeight;
    const int boxTopMargin = 10;
    const int boxLeftMargin = 10;
    const int boxHorizontalSpacing = 30;
    const int verticalBoxDistance = spacing + boxHeight;

    int ret;
    // XXX: use the type string to look up a user defined type
    ret = val_resolve_and_check(NULL, name.toAscii().data(), 1, ns_t_a,
                                VAL_QUERY_RECURSE | VAL_QUERY_AC_DETAIL |
                                VAL_QUERY_SKIP_CACHE,
                                &results);
    if (ret != 0 || !results) {
        qWarning() << "failed to get results..."; // XXX: display SOMETHING!
        return;
    }

    int spot = 0;
    int maxWidth = 0;
    QGraphicsRectItem        *rect = 0;
    QGraphicsSimpleTextItem  *text;
    struct val_rr_rec *rrrec;
    const u_char * rdata;

    QMap<int, QPair<int, int> > dnskeyIdToLocation;
    QMap<QPair<int, int>, QPair<int, int> > dsIdToLocation;

    // for each authentication record, display a horiz row of data
    for(vrcptr = results->val_rc_answer; vrcptr; vrcptr = vrcptr->val_ac_trust) {
        int horizontalSpot = boxLeftMargin;

        // for each rrset in an auth record, display a box
        qDebug() << "chain: " << vrcptr->val_ac_rrset->val_rrset_name << " -> " << vrcptr->val_ac_rrset->val_rrset_type;

        for(rrrec = vrcptr->val_ac_rrset->val_rrset_data; rrrec; rrrec = rrrec->rr_next) {
            QString nextLineText;

            rdata = rrrec->rr_rdata;

            // draw the bounding box of the record
            rect = new QGraphicsRectItem(horizontalSpot, spot+boxTopMargin, boxWidth, boxHeight);
            rect->setPen(QPen(Qt::black));
            myScene->addItem(rect);

            nextLineText = "%1 (%2)";
            // add the type-line
            if (m_typeToName.contains(vrcptr->val_ac_rrset->val_rrset_type))
                nextLineText = nextLineText.arg(m_typeToName[vrcptr->val_ac_rrset->val_rrset_type]);
            else
                nextLineText = nextLineText.arg("(type unknown)");

            if (rrrec->rr_status == VAL_AC_UNSET)
                nextLineText = nextLineText.arg("");
            if (m_statusToName.contains(rrrec->rr_status))
                nextLineText = nextLineText.arg(m_statusToName[rrrec->rr_status]);
            else
                nextLineText = nextLineText.arg("status unknown");

            text = new QGraphicsSimpleTextItem(nextLineText);
            text->setPen(QPen(Qt::black));
            text->setPos(boxLeftMargin + horizontalSpot, spot + boxTopMargin*2);
            text->setScale(2.0);
            myScene->addItem(text);

            // add the domain line
            QString rrsetName = vrcptr->val_ac_rrset->val_rrset_name;
            text = new QGraphicsSimpleTextItem(rrsetName == "." ? "<root>" : rrsetName);
            text->setPen(QPen(Qt::black));
            text->setPos(boxLeftMargin + horizontalSpot, spot + boxHeight/2);
            text->setScale(2.0);
            myScene->addItem(text);

            int     keyId;
            u_int   keyflags, protocol, algorithm, digest_type;

            switch (vrcptr->val_ac_rrset->val_rrset_type) {
            case ns_t_dnskey:
                if (rrrec->rr_rdata_length < 0U + NS_INT16SZ + NS_INT8SZ + NS_INT8SZ)
                    break;

                /* grab the KeyID */
                keyId = id_calc(rrrec->rr_rdata, rrrec->rr_rdata_length);

                /* get the flags */
                RES_GET16(keyflags, rrrec->rr_rdata);
                protocol = *rdata++;
                algorithm = *rdata++;

                nextLineText = QString(tr("%1, id: %2, proto: %3, alg: %4"))
                        .arg((keyflags & 0x1) ? "KSK" : "ZSK")
                        .arg(keyId)
                        .arg(protocol)
                        .arg(algorithm);
                dnskeyIdToLocation[keyId] = QPair<int,int>(horizontalSpot, spot + boxTopMargin);
                break;

            case ns_t_ds:
                RES_GET16(keyId, rdata);
                algorithm = *rdata++ & 0xF;
                digest_type = *rdata++ & 0xF;

                nextLineText = QString(tr("id: %1, alg: %2, digest: %3"))
                        .arg(keyId)
                        .arg(algorithm)
                        .arg(digest_type);
                dsIdToLocation[QPair<int, int>(keyId, digest_type)] = QPair<int,int>(horizontalSpot, spot + boxTopMargin);

                break;
            }


            if (nextLineText.length() > 0) {
                text = new QGraphicsSimpleTextItem(nextLineText);
                text->setPen(QPen(Qt::black));
                text->setPos(boxLeftMargin + horizontalSpot, spot + boxHeight - boxTopMargin*3);
                text->setScale(2.0);
                myScene->addItem(text);
            }

            horizontalSpot += boxWidth + boxHorizontalSpacing;
            maxWidth = qMax(maxWidth, horizontalSpot);
        }

        spot -= verticalBoxDistance;
    }

    // loop through all the DS records and have them point to the keys they're referencing
    QMap<QPair<int, int>, QPair<int, int> >::iterator dsIter, dsEnd = dsIdToLocation.end();
    for(dsIter = dsIdToLocation.begin(); dsIter != dsEnd; dsIter++) {
        QPair<int, int> dsLocation = dsIter.value();
        QPair<int, int> dnskeyLocation = dnskeyIdToLocation[dsIter.key().first];
        drawArrow(dsLocation.first + boxWidth/2, dsLocation.second + boxHeight,
                  dnskeyLocation.first + boxWidth/2, dnskeyLocation.second);
    }

    myScene->setSceneRect(0, spot + boxHeight, maxWidth, -spot + boxHeight);
    if (rect)
        ensureVisible(rect);
}