/* -*- Mode: Java; tab-width: 4 -*-
 *
 * Copyright (c) 2004 Apple Computer, Inc. All rights reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 * 
 *     http://www.apache.org/licenses/LICENSE-2.0
 * 
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.

    Change History (most recent first):

$Log: DNSSDException.java,v $
Revision 1.5  2007/02/07 01:19:36  cheshire
<rdar://problem/4849427> API: Reconcile conflicting error code values

Revision 1.4  2006/08/14 23:25:08  cheshire
Re-licensed mDNSResponder daemon source code under Apache License, Version 2.0

Revision 1.3  2005/07/10 22:19:01  cheshire
Add missing error codes to list of public static final ints

Revision 1.2  2004/04/30 21:48:27  rpantos
Change line endings for CVS.

Revision 1.1  2004/04/30 16:29:35  rpantos
First checked in.

*/

package	com.apple.dnssd;


/**
	Used to report various DNS-SD-related error conditions.
*/

abstract public class	DNSSDException extends Exception
{
    public static final int		NO_ERROR                  =  0;
    public static final int		UNKNOWN                   = -65537;
    public static final int		NO_SUCH_NAME              = -65538;
    public static final int		NO_MEMORY                 = -65539;
    public static final int		BAD_PARAM                 = -65540;
    public static final int		BAD_REFERENCE             = -65541;
    public static final int		BAD_STATE                 = -65542;
    public static final int		BAD_FLAGS                 = -65543;
    public static final int		UNSUPPORTED               = -65544;
    public static final int		NOT_INITIALIZED           = -65545;
    public static final int		NO_CACHE                  = -65546;
    public static final int		ALREADY_REGISTERED        = -65547;
    public static final int		NAME_CONFLICT             = -65548;
    public static final int		INVALID                   = -65549;
    public static final int		FIREWALL                  = -65550;
    public static final int		INCOMPATIBLE              = -65551;
    public static final int		BAD_INTERFACE_INDEX       = -65552;
    public static final int		REFUSED                   = -65553;
    public static final int		NOSUCHRECORD              = -65554;
    public static final int		NOAUTH                    = -65555;
    public static final int		NOSUCHKEY                 = -65556;
    public static final int		NATTRAVERSAL              = -65557;
    public static final int		DOUBLENAT                 = -65558;
    public static final int		BADTIME                   = -65559;
    public static final int		BADSIG                    = -65560;
    public static final int		BADKEY                    = -65561;
    public static final int		TRANSIENT                 = -65562;
    public static final int		SERVICENOTRUNNING         = -65563;
    public static final int		NATPORTMAPPINGUNSUPPORTED = -65564;
    public static final int		NATPORTMAPPINGDISABLED    = -65565;

	/** Returns the sub-code that identifies the particular error. */
	abstract public int			getErrorCode();
}

