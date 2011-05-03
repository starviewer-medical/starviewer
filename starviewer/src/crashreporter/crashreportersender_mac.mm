#import "crashreportersender.h"

#import <Foundation/Foundation.h>
#import "common/mac/HTTPMultipartUpload.h"


namespace udg {


bool CrashReporterSender::sendReport(QString url, QString minidumpPath, QHash<QString,QString> &options)
{
 
    NSString *urlString = [NSString stringWithUTF8String:url.toUtf8().constData()];
    NSString *minidumpPathString = [NSString stringWithUTF8String:minidumpPath.toUtf8().constData()];
    NSURL *uploadUrl = [NSURL URLWithString:urlString];
    HTTPMultipartUpload *ul = [[HTTPMultipartUpload alloc] initWithURL:uploadUrl];
    NSMutableDictionary *parameters = [NSMutableDictionary dictionary];
    
    NSLog(@"Parameters:\n");
    Q_FOREACH(QString key, options.keys())
    {
        NSString *keyString = [NSString stringWithUTF8String:key.toUtf8().constData()];
        NSString *valueString = [NSString stringWithUTF8String:options.take(key).toUtf8().constData()];
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
