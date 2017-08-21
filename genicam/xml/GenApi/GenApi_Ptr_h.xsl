<?xml version="1.0" encoding="utf-8"?>
<!-- ***************************************************************************
*  (c) 2004-2008 by Basler Vision Technologies
*  Section: Vision Components
*  Project: GenApi
*  Author:  Urs Helmig
*  $Header$
*
*  License: This file is published under the license of the EMVA GenICam  Standard Group. 
*  A text file describing the legal terms is included in  your installation as 'GenICam_license.pdf'. 
*  If for some reason you are missing  this file please contact the EMVA or visit the website
*  (http://www.genicam.org) for a full copy.
* 
*  THIS SOFTWARE IS PROVIDED BY THE EMVA GENICAM STANDARD GROUP "AS IS"
*  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,  
*  THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR  
*  PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE EMVA GENICAM STANDARD  GROUP 
*  OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,  SPECIAL, 
*  EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT  LIMITED TO, 
*  PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,  DATA, OR PROFITS; 
*  OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY  THEORY OF LIABILITY, 
*  WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT  (INCLUDING NEGLIGENCE OR OTHERWISE) 
*  ARISING IN ANY WAY OUT OF THE USE  OF THIS SOFTWARE, EVEN IF ADVISED OF THE 
*  POSSIBILITY OF SUCH DAMAGE.
******************************************************************************** -->
<xsl:stylesheet version="1.0" xmlns:my_v1_0="http://www.genicam.org/GenApi/Version_1_0" xmlns:my_v1_1="http://www.genicam.org/GenApi/Version_1_1" xmlns:xsl="http://www.w3.org/1999/XSL/Transform">
<xsl:output method="text" encoding="UTF-8" media-type="text/plain"/>

<xsl:param name="HeaderFileName"><xsl:value-of select="/RegisterDescription/@ModelName"/><xsl:text>Params.h</xsl:text></xsl:param>

<xsl:template match="/">
<!-- =========================================================================== -->
<!-- Main body                                                                   -->
<!-- =========================================================================== -->
//-----------------------------------------------------------------------------
//  (c) 2004-2008 by Basler Vision Technologies
//  Section: Vision Components
//  Project: GenApi
//-----------------------------------------------------------------------------
/*!
\file
\brief <xsl:value-of select="/RegisterDescription/@ToolTip"/>
*/
//-----------------------------------------------------------------------------
//  This file is generated automatically
//  Do not modify!
//-----------------------------------------------------------------------------
<xsl:variable name="NameSpace">
    <xsl:value-of select="/RegisterDescription/@VendorName"/>_<xsl:value-of select="/RegisterDescription/@ModelName"/>
</xsl:variable>

#ifndef <xsl:value-of select="$NameSpace"/><xsl:text>_PTR_H</xsl:text>
#define <xsl:value-of select="$NameSpace"/><xsl:text>_PTR_H</xsl:text>

#include &lt;GenApi/NodeMapRef.h&gt;
#include "<xsl:value-of select="$HeaderFileName"/>"

//! The namespace containing the device's control interface and related enumeration types
namespace <xsl:value-of select="$NameSpace"/>
{
    //**************************************************************************************************
    // Access class
    //**************************************************************************************************
    //! <xsl:value-of select="/RegisterDescription/@ToolTip"/>
    class C<xsl:value-of select="/RegisterDescription/@ModelName"/>
        : public GenApi::CNodeMapRefT&lt;<xsl:value-of select="$NameSpace"/>::C<xsl:value-of select="/RegisterDescription/@ModelName"/>_Params&gt;
    {
    public:
        //! Constructor
        C<xsl:value-of select="/RegisterDescription/@ModelName"/>(GenICam::gcstring DeviceName = "Device") : GenApi::CNodeMapRefT&lt;<xsl:value-of select="$NameSpace"/>::C<xsl:value-of select="/RegisterDescription/@ModelName"/>_Params&gt;(DeviceName)
        {
        }
    };


} // namespace <xsl:value-of select="$NameSpace"/>

#endif // <xsl:value-of select="$NameSpace"/><xsl:text>_PTR_H</xsl:text>
<xsl:text>
</xsl:text>

</xsl:template>
</xsl:stylesheet>
