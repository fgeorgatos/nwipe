/*
 *  create_pdf.c: Routines that create the PDF erasure certificate
 *
 *  Copyright PartialVolume <https://github.com/PartialVolume>.
 *
 *  This program is free software; you can redistribute it and/or modify it under
 *  the terms of the GNU General Public License as published by the Free Software
 *  Foundation, version 2.
 *
 *  This program is distributed in the hope that it will be useful, but WITHOUT
 *  ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 *  FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
 *  details.
 *
 *  You should have received a copy of the GNU General Public License along with
 *  this program; if not, write to the Free Software Foundation, Inc.,
 *  51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 *
 */

#include <stdint.h>
#include "stdarg.h"
#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "nwipe.h"
#include "context.h"
#include "create_pdf.h"
#include "PDFGen/pdfgen.h"
#include "version.h"
#include "method.h"
#include "embedded_images/shred_db.jpg.h"
#include "embedded_images/tick_erased.jpg.h"
#include "logging.h"
#include "options.h"

int create_pdf( nwipe_context_t* ptr )
{
    char pdf_footer[MAX_PDF_FOOTER_TEXT_LENGTH];
    nwipe_context_t* c;
    c = ptr;
    char model_header[50]; /* Model text in the header */
    char serial_header[30]; /* Serial number text in the header */
    char barcode[80]; /* Contents of the barcode, i.e model:serial */
    char dimm[50]; /* Disk Information: Model */
    char verify[20]; /* Verify option text */

    struct pdf_info info = { .creator = "https://github.com/PartialVolume/shredos.x86_64",
                             .producer = "https://github.com/martijnvanbrummelen/nwipe",
                             .title = "PDF Disk Erasure Certificate",
                             .author = "Nwipe",
                             .subject = "Disk Erase Certificate",
                             .date = "Today" };

    // nwipe_log( NWIPE_LOG_NOTICE, "Create the PDF disk erasure certificate" );
    struct pdf_doc* pdf = pdf_create( PDF_A4_WIDTH, PDF_A4_HEIGHT, &info );

    /* Create footer text string and append the version */
    strcpy( pdf_footer, "Disc Erasure by NWIPE version" );
    strcat( pdf_footer, version_string );

    pdf_set_font( pdf, "Helvetica" );
    pdf_append_page( pdf );

    /* Create header and footer*/
    pdf_add_text( pdf, NULL, pdf_footer, 12, 200, 30, PDF_BLACK );
    pdf_add_line( pdf, NULL, 50, 50, 550, 50, 3, PDF_BLACK );
    pdf_add_line( pdf, NULL, 50, 650, 550, 650, 3, PDF_BLACK );
    pdf_add_image_data( pdf, NULL, 45, 670, 128, 128, bin2c_shred_db_jpg, 27063 );
    pdf_add_image_data( pdf, NULL, 430, 670, 128, 128, bin2c_te_jpg, 54896 );
    snprintf( model_header, sizeof( model_header ), " %s:%s ", "Model", c->device_model );
    pdf_add_text( pdf, NULL, model_header, 14, 215, 720, PDF_BLACK );
    snprintf( serial_header, sizeof( serial_header ), " %s:%s ", "S/N", c->device_serial_no );
    pdf_add_text( pdf, NULL, serial_header, 14, 212, 700, PDF_BLACK );
    pdf_add_text( pdf, NULL, "Disk Erasure Report", 24, 195, 670, PDF_BLACK );
    snprintf( barcode, sizeof( barcode ), " %s:%s ", c->device_model, c->device_serial_no );
    pdf_add_barcode( pdf, NULL, PDF_BARCODE_128A, 195, 748, 200, 50, barcode, PDF_BLACK );

    /* Organisation Information */
    pdf_add_line( pdf, NULL, 50, 550, 550, 550, 1, PDF_GRAY );
    pdf_add_text( pdf, NULL, "Organisation Performing The Disk Erasure", 12, 50, 630, PDF_BLUE );
    pdf_add_text( pdf, NULL, "Business Name:", 12, 60, 610, PDF_GRAY );
    pdf_add_text( pdf, NULL, "Business Address:", 12, 60, 590, PDF_GRAY );
    pdf_add_text( pdf, NULL, "Contact Name:", 12, 60, 570, PDF_GRAY );
    pdf_add_text( pdf, NULL, "Contact Phone:", 12, 300, 570, PDF_GRAY );

    /* Customer Information */
    pdf_add_line( pdf, NULL, 50, 450, 550, 450, 1, PDF_GRAY );
    pdf_add_text( pdf, NULL, "Customer Details", 12, 50, 530, PDF_BLUE );
    pdf_add_text( pdf, NULL, "Name:", 12, 60, 510, PDF_GRAY );
    pdf_add_text( pdf, NULL, "Address:", 12, 60, 490, PDF_GRAY );
    pdf_add_text( pdf, NULL, "Contact Name:", 12, 60, 470, PDF_GRAY );
    pdf_add_text( pdf, NULL, "Contact Phone:", 12, 300, 470, PDF_GRAY );

    /* Technician/Operator ID */
    pdf_add_line( pdf, NULL, 50, 390, 550, 390, 1, PDF_GRAY );
    pdf_add_text( pdf, NULL, "Technician/Operator ID", 12, 50, 430, PDF_BLUE );
    pdf_add_text( pdf, NULL, "Name:", 12, 60, 410, PDF_GRAY );
    pdf_add_text( pdf, NULL, "ID:", 12, 300, 410, PDF_GRAY );

    /* Disk Information */
    pdf_add_line( pdf, NULL, 50, 270, 550, 270, 1, PDF_GRAY );
    pdf_add_text( pdf, NULL, "Disk Information", 12, 50, 370, PDF_BLUE );

    pdf_add_text( pdf, NULL, "Make/Model:", 12, 60, 350, PDF_GRAY );
    pdf_add_text( pdf, NULL, c->device_model, 12, 135, 350, PDF_BLACK );

    pdf_add_text( pdf, NULL, "Serial:", 12, 300, 350, PDF_GRAY );
    pdf_add_text( pdf, NULL, c->device_serial_no, 12, 350, 350, PDF_GRAY );

    pdf_add_text( pdf, NULL, "Size:", 12, 60, 330, PDF_GRAY );
    pdf_add_text( pdf, NULL, c->device_size_text, 12, 85, 330, PDF_BLACK );

    pdf_add_text( pdf, NULL, "Bus:", 12, 300, 330, PDF_GRAY );
    pdf_add_text( pdf, NULL, "Health:", 12, 60, 310, PDF_GRAY );
    pdf_add_text( pdf, NULL, "Remapped Sectors:", 12, 300, 310, PDF_GRAY );
    pdf_add_text( pdf, NULL, "HPA(pre-erase):", 12, 60, 290, PDF_GRAY );
    pdf_add_text( pdf, NULL, "DCO(pre-erase):", 12, 300, 290, PDF_GRAY );

    /* Erasure Details */
    pdf_add_text( pdf, NULL, "Disk Erasure Details", 12, 50, 250, PDF_BLUE );
    pdf_add_text( pdf, NULL, "Start/End Time:", 12, 60, 230, PDF_GRAY );
    pdf_add_text( pdf, NULL, "Duration:", 12, 300, 230, PDF_GRAY );
    pdf_add_text( pdf, NULL, "Method:", 12, 60, 210, PDF_GRAY );
    pdf_add_text( pdf, NULL, nwipe_method_label( nwipe_options.method ), 12, 110, 210, PDF_BLACK );
    pdf_add_text( pdf, NULL, "PRNG algorithm:", 12, 300, 210, PDF_GRAY );
    pdf_add_text( pdf, NULL, "Final Pass(Zeros/Ones/None):", 12, 60, 190, PDF_GRAY );

    /* Verify abbreviations used in summary table */
    switch( nwipe_options.verify )
    {
        case NWIPE_VERIFY_NONE:
            strcpy( verify, "Verify None" );
            break;

        case NWIPE_VERIFY_LAST:
            strcpy( verify, "Verify Last" );
            break;

        case NWIPE_VERIFY_ALL:
            strcpy( verify, "Verify All" );
            break;
    }
    pdf_add_text( pdf, NULL, "Verify Pass(Last/All/None):", 12, 300, 190, PDF_GRAY );
    pdf_add_text( pdf, NULL, verify, 12, 450, 190, PDF_BLACK );

    pdf_add_text( pdf, NULL, "Status:", 12, 60, 170, PDF_GRAY );
    pdf_add_text( pdf, NULL, "Rounds:", 12, 300, 170, PDF_GRAY );
    pdf_add_text( pdf, NULL, "HPA(post-erase):", 12, 60, 150, PDF_GRAY );
    pdf_add_text( pdf, NULL, "DCO(post-erase):", 12, 300, 150, PDF_GRAY );
    pdf_add_text( pdf, NULL, "Information:", 12, 60, 130, PDF_GRAY );

    /* Certificate Date/Time */

    pdf_save( pdf, "output.pdf" );
    pdf_destroy( pdf );
    // nwipe_log( NWIPE_LOG_NOTICE, "PDF disk erasure certificate sucessfully created." );
    return 0;
}