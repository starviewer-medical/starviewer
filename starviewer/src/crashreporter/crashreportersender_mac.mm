/***************************************************************************
 *   Copyright (C) 2005 by Grup de Gràfics de Girona                       *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/

#import "crashreportersender.h"

#import <Foundation/Foundation.h>
#import "../main/src_breakpad/common/mac/HTTPMultipartUpload.h"


namespace udg {


bool CrashReporterSender::sendReport(QString url, QString minidumpPath, QHash<QString,QString> &options)
{
 
    NSString *urlString = [NSString stringWithCString:url.toStdString().c_str()];
    NSString *minidumpPathString = [NSString stringWithCString:minidumpPath.toStdString().c_str()];
    NSURL *uploadUrl = [NSURL URLWithString:urlString];
    HTTPMultipartUpload *ul = [[HTTPMultipartUpload alloc] initWithURL:uploadUrl];
    NSMutableDictionary *parameters = [NSMutableDictionary dictionary];
    
    NSLog(@"Parameters:\n");
    foreach(QString key, options.keys())
    {
        NSString *keyString = [NSString stringWithCString:key.toStdString().c_str()];
        NSString *valueString = [NSString stringWithCString:options.take(key).toStdString().c_str()];
        [parameters setObject:valueString forKey:keyString];
        NSLog(@"Key: %@ Value: %@\n", keyString, valueString);
    }

    [ul setParameters:parameters];
    
    // Add file
    [ul addFileAtPath:minidumpPathString name:@"upload_file_minidump"];
    
    // Send it
    NSError *error = nil;
    NSData *data = [ul send:&error];
    NSString *result = [[NSString alloc] initWithData:data
                                             encoding:NSUTF8StringEncoding];
    
    NSLog(@"Send: %@", error ? [error description] : @"No Error");
    NSLog(@"Response: %d", [[ul response] statusCode]);
    NSLog(@"Result: %d bytes\n%@", [data length], result);
    
    [result release];
    [ul release];
    
    if ( error )
        return false;
    else
        return true;
}


};