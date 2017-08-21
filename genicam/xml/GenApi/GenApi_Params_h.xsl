<?xml version="1.0" encoding="utf-8"?>
<!-- ***************************************************************************
*  (c) 2004-2008 by Basler Vision Technologies
*  Section: Vision Components
*  Project: GenApi
*  Author:  Fritz Dierks
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

#ifndef <xsl:value-of select="$NameSpace"/><xsl:text>_PARAMS_H</xsl:text>
#define <xsl:value-of select="$NameSpace"/><xsl:text>_PARAMS_H</xsl:text>

#include &lt;GenApi/IEnumerationT.h&gt;
#include &lt;GenApi/NodeMapRef.h&gt;
#include &lt;GenApi/DLLLoad.h&gt;

//! The namespace containing the device's control interface and related enumeration types
namespace <xsl:value-of select="$NameSpace"/>
{

    //**************************************************************************************************
    // Enumerations
    //**************************************************************************************************
    <xsl:apply-templates select="/RegisterDescription/Enumeration" mode="DefiningEnums"/>

    //**************************************************************************************************
    // Parameter class
    //**************************************************************************************************
    <xsl:variable name="Class">
        <xsl:text>C</xsl:text><xsl:value-of select="/RegisterDescription/@ModelName"/><xsl:text>_Params</xsl:text>
    </xsl:variable>

    //! <xsl:value-of select="/RegisterDescription/@ToolTip"/>
    class <xsl:value-of select="$Class"/>
    {
    //----------------------------------------------------------------------------------------------------------------
    // Implementation
    //----------------------------------------------------------------------------------------------------------------
    protected:
		// If you want to show the following methods in the help file
		// add the string HIDE_CLASS_METHODS to the ENABLED_SECTIONS tag in the doxygen file
        //! \cond HIDE_CLASS_METHODS
        
            //! Constructor
            <xsl:value-of select="$Class"/>(void);

            //! Destructor
            ~<xsl:value-of select="$Class"/>(void);

            //! Initializes the references
            void _Initialize(GenApi::INodeMap*);

            //! Return the vendor of the camera
            const char* _GetVendorName(void);

            //! Returns the camera model name
            const char* _GetModelName(void);
        
        //! \endcond

    //----------------------------------------------------------------------------------------------------------------
    // References to features
    //----------------------------------------------------------------------------------------------------------------
    public:
        <xsl:apply-templates select="/RegisterDescription/*[ @ExposeStatic='Yes' or ( (/RegisterDescription/Category/pFeature=@Name and name()!='Category') and not(@ExposeStatic='No') ) ]" mode="ReferenceDeclaration"/>

    private:
        //! \cond HIDE_CLASS_METHODS

            //! not implemented copy constructor
            <xsl:value-of select="$Class"/>(<xsl:value-of select="$Class"/>&amp;);

            //! not implemented assignment operator
            <xsl:value-of select="$Class"/>&amp; operator=(<xsl:value-of select="$Class"/>&amp;);

        //! \endcond
    };


    //**************************************************************************************************
    // Parameter class implementation
    //**************************************************************************************************

    //! \cond HIDE_CLASS_METHODS

    inline <xsl:value-of select="$Class"/>::<xsl:value-of select="$Class"/>(void)
        <xsl:apply-templates select="/RegisterDescription/*[ @ExposeStatic='Yes' or ( (/RegisterDescription/Category/pFeature=@Name and name()!='Category') and not(@ExposeStatic='No') ) ]" mode="ReferenceCreation"/>
    {
    }

    inline <xsl:value-of select="$Class"/>::~<xsl:value-of select="$Class"/>(void)
    {
        <xsl:apply-templates select="/RegisterDescription/*[ @ExposeStatic='Yes' or ( (/RegisterDescription/Category/pFeature=@Name and name()!='Category') and not(@ExposeStatic='No') ) ]" mode="ReferenceDeletion"/>
    }

    inline void <xsl:value-of select="$Class"/>::_Initialize(GenApi::INodeMap* _Ptr)
    {
        <xsl:apply-templates select="/RegisterDescription/*[ @ExposeStatic='Yes' or ( (/RegisterDescription/Category/pFeature=@Name and name()!='Category') and not(@ExposeStatic='No') ) ]" mode="ReferenceInitialization"/>
    }

    inline const char* <xsl:value-of select="$Class"/>::_GetVendorName(void)
    {
        return "<xsl:value-of select="RegisterDescription/@VendorName"/>";
    }

    inline const char* <xsl:value-of select="$Class"/>::_GetModelName(void)
    {
        return "<xsl:value-of select="RegisterDescription/@ModelName"/>";
    }

    //! \endcond

} // namespace <xsl:value-of select="$NameSpace"/>

#endif // <xsl:value-of select="$NameSpace"/><xsl:text>_PARAMS_H</xsl:text>

<xsl:text>
</xsl:text>
    
</xsl:template>


<!-- =========================================================================== -->
<!-- DefiningEnums                                                               -->
<!-- =========================================================================== -->

<xsl:template match="Enumeration" mode="DefiningEnums">
    //! Valid values for <xsl:value-of select="@Name"/>
    enum <xsl:value-of select="@Name"/>Enums
    {
        <xsl:apply-templates select="./pEnumEntry" mode="InsideDefiningEnums"/>
    };
</xsl:template>

<!-- =========================================================================== -->
<!-- InsideDefiningEnums                                                         -->
<!-- =========================================================================== -->
        
<xsl:template match="pEnumEntry" mode="InsideDefiningEnums">
    <xsl:variable name="NodeName" select="string()"/>
    <xsl:variable name="Separator">
        <xsl:choose>
            <xsl:when test="position()!=last()">
                <xsl:text>, </xsl:text>
            </xsl:when>
            <xsl:otherwise>
                <xsl:text>  </xsl:text>
            </xsl:otherwise>
        </xsl:choose>
    </xsl:variable>
    <xsl:value-of select="../@Name"/>_<xsl:value-of select="../../EnumEntry[@Name=$NodeName]/Symbolic"/><xsl:value-of select="$Separator"/> //!&lt;<xsl:value-of select="../../EnumEntry[@Name=$NodeName]/ToolTip"/><xsl:text>&#x0d;&#x0a;&#x9;&#x9;</xsl:text>
</xsl:template>

<!-- =========================================================================== -->
<!-- ReferenceDeclaration                                                        -->
<!-- =========================================================================== -->

<xsl:template match="*" mode="ReferenceDeclaration">
    <xsl:variable name="Feature" select="@Name"/>
    <xsl:variable name="NodeType" select="name()"/>

    <xsl:choose>
        <xsl:when test="/RegisterDescription/Category[pFeature=$Feature]">
	//! \name <xsl:value-of select="/RegisterDescription/Category[pFeature=$Feature]/@Name"/> - <xsl:value-of select="/RegisterDescription/Category[pFeature=$Feature]/ToolTip"/>
        </xsl:when>
        <xsl:otherwise>
	//! \name Miscellaneous Features
        </xsl:otherwise>        
    </xsl:choose>
    //@{
	/*!	
		\brief <xsl:value-of select="/RegisterDescription/*[@Name=$Feature]/ToolTip"/>
		<xsl:text>&#x0d;&#x0a;&#x0d;&#x0a;&#x9;</xsl:text>
		<xsl:value-of select="/RegisterDescription/*[@Name=$Feature]/Description"/>
		<xsl:text>&#x0d;&#x0a;&#x9;</xsl:text>
    <xsl:choose>
        <xsl:when test="/RegisterDescription/*[@Name=$Feature]/Visibility">
		\b Visibility = <xsl:value-of select="/RegisterDescription/*[@Name=$Feature]/Visibility"/>
        </xsl:when>
        <xsl:otherwise>
		\b Visibility = Beginner
        </xsl:otherwise>        
    </xsl:choose><xsl:text>&#x0d;&#x0a;&#x9;</xsl:text>
	<xsl:if test="/RegisterDescription/*[@Name=$Feature]/pSelecting">
		\b Selected by : <xsl:for-each select="/RegisterDescription/*[@Name=$Feature]/pSelecting">
			<xsl:value-of select="node()"/>
			<xsl:if test="position()!=last()">, </xsl:if>
		</xsl:for-each>
		<xsl:text>&#x0d;&#x0a;&#x9;</xsl:text>
	</xsl:if>
    */
    <xsl:if test="/RegisterDescription/*[@Name=$Feature]/IsDeprecated='Yes'">
#   pragma deprecated( <xsl:value-of select="$Feature"/> )
#   pragma warning(push)
#   pragma warning(disable: 4995) // name has been marked as deprecated
    </xsl:if>  
    <xsl:choose>
        <xsl:when test="$NodeType = 'Enumeration'">
            <xsl:text>GenApi::</xsl:text>
            <xsl:value-of select="document('NodeTypes.xml')/NodeTypes/Node/CppInterface[../@Name=$NodeType]"/><xsl:text>T&lt;</xsl:text>
            <xsl:value-of select="$Feature"/>Enums <xsl:text>&gt;</xsl:text>
        </xsl:when>
        <xsl:otherwise>
            <xsl:text>GenApi::</xsl:text>
            <xsl:value-of select="document('NodeTypes.xml')/NodeTypes/Node/CppInterface[../@Name=$NodeType]"/>
        </xsl:otherwise>
    </xsl:choose>
    <xsl:text> &amp;</xsl:text>
    <xsl:value-of select="$Feature"/>;
    <xsl:if test="/RegisterDescription/*[@Name=$Feature]/IsDeprecated='Yes'">
#   pragma warning(pop)
    </xsl:if>
    //@}

</xsl:template>


<!-- =========================================================================== -->
<!-- ReferenceCreation                                                           -->
<!-- =========================================================================== -->

<xsl:template match="*" mode="ReferenceCreation">
    <xsl:variable name="Feature" select="@Name"/>
    <xsl:variable name="NodeType" select="name()"/>

    <xsl:variable name="Separator">
        <xsl:choose>
            <xsl:when test="position()=1">
                <xsl:text>: </xsl:text>
            </xsl:when>
            <xsl:otherwise>
                <xsl:text>, </xsl:text>
            </xsl:otherwise>
        </xsl:choose>
    </xsl:variable>
    <xsl:if test="./IsDeprecated='Yes'">
#		pragma warning(push)
#		pragma warning(disable: 4995) // name has been marked as deprecated
		</xsl:if>
    <xsl:choose>
        <xsl:when test="$NodeType = 'Enumeration'">
            <xsl:value-of select="$Separator"/><xsl:value-of select="$Feature"/>( *new GenApi::<xsl:value-of select="document('NodeTypes.xml')/NodeTypes/Node/CppReference[../@Name=$NodeType]"/>&lt;<xsl:value-of select="$Feature"/>Enums&gt;() )
        </xsl:when>
        <xsl:otherwise>
            <xsl:value-of select="$Separator"/><xsl:value-of select="$Feature"/>( *new GenApi::<xsl:value-of select="document('NodeTypes.xml')/NodeTypes/Node/CppReference[../@Name=$NodeType]"/>() )
        </xsl:otherwise>
    </xsl:choose>
    <xsl:if test="./IsDeprecated='Yes'">
#		pragma warning(pop)
		</xsl:if>
</xsl:template>


<!-- =========================================================================== -->
<!-- ReferenceDeletion                                                           -->
<!-- =========================================================================== -->

<xsl:template match="*" mode="ReferenceDeletion">
    <xsl:variable name="Feature" select="@Name"/>
    <xsl:variable name="NodeType" select="name()"/>
    <xsl:if test="./IsDeprecated='Yes'">
#		pragma warning(push)
#		pragma warning(disable: 4995) // name has been marked as deprecated
		</xsl:if>
    <xsl:choose>
        <xsl:when test="$NodeType = 'Enumeration'">
		<xsl:text>delete static_cast &lt; GenApi::</xsl:text><xsl:value-of select="document('NodeTypes.xml')/NodeTypes/Node/CppReference[../@Name=$NodeType]"/>&lt;<xsl:value-of select="$Feature"/>Enums&gt; *&gt; (&amp;<xsl:value-of select="$Feature"/> );
        </xsl:when>
        <xsl:otherwise>
        <xsl:text>delete static_cast &lt; GenApi::</xsl:text><xsl:value-of select="document('NodeTypes.xml')/NodeTypes/Node/CppReference[../@Name=$NodeType]"/>*&gt; (&amp;<xsl:value-of select="$Feature"/> );
        </xsl:otherwise>
    </xsl:choose>
    <xsl:if test="./IsDeprecated='Yes'">
#		pragma warning(pop)
		</xsl:if>
</xsl:template>

<!-- =========================================================================== -->
<!-- ReferenceInitialization                                                     -->
<!-- =========================================================================== -->

<xsl:template match="*" mode="ReferenceInitialization">
	<xsl:variable name="Feature" select="@Name"/>
	<xsl:variable name="NodeType" select="name()"/>
	<xsl:if test="./IsDeprecated='Yes'">
#	pragma warning(push)
#	pragma warning(disable: 4995) // name has been marked as deprecated
	</xsl:if>
  <xsl:choose>
    <xsl:when test="$NodeType = 'Enumeration'">
      <xsl:text>static_cast&lt;GenApi::</xsl:text><xsl:value-of select="document('NodeTypes.xml')/NodeTypes/Node/CppReference[../@Name=$NodeType]"/>&lt;<xsl:value-of select="$Feature"/>Enums&gt; *&gt; (&amp;<xsl:value-of select="$Feature"/> )<xsl:text>-&gt;SetReference(_Ptr-&gt;GetNode("</xsl:text><xsl:value-of select="$Feature"/>"));
    </xsl:when>
    <xsl:otherwise>
      <xsl:text>static_cast&lt;GenApi::</xsl:text><xsl:value-of select="document('NodeTypes.xml')/NodeTypes/Node/CppReference[../@Name=$NodeType]"/>*&gt; (&amp;<xsl:value-of select="$Feature"/> )<xsl:text>-&gt;SetReference(_Ptr-&gt;GetNode("</xsl:text><xsl:value-of select="$Feature"/>"));
    </xsl:otherwise>
  </xsl:choose>

  <xsl:if test="$NodeType = 'Enumeration'">
    <xsl:text>static_cast&lt;GenApi::</xsl:text><xsl:value-of select="document('NodeTypes.xml')/NodeTypes/Node/CppReference[../@Name=$NodeType]"/>&lt;<xsl:value-of select="$Feature"/>Enums&gt; *&gt; (&amp;<xsl:value-of select="$Feature"/> )<xsl:text>-&gt;SetNumEnums(</xsl:text><xsl:value-of select="count(/RegisterDescription/*[@Name=$Feature]/pEnumEntry)"/>);<xsl:text>&#x0d;&#x0a;&#x9;</xsl:text>
	<xsl:apply-templates select="/RegisterDescription/*[@Name=$Feature]/pEnumEntry" mode="EnumInitialization">
		<xsl:with-param name="EnumName" select="$Feature"/>
    <xsl:with-param name="NodeType" select="$NodeType"/>
  </xsl:apply-templates>
	</xsl:if>
	<xsl:if test="./IsDeprecated='Yes'">
#	pragma warning(pop)
	</xsl:if>
</xsl:template>

<!-- =========================================================================== -->
<!-- EnumInitialization                                                          -->
<!-- =========================================================================== -->

<xsl:template match="*" mode="EnumInitialization">
	<xsl:param name="EnumName"/>
	<xsl:variable name="FeatureName" select="string()"/>
	<xsl:apply-templates select="/RegisterDescription/*[@Name=$FeatureName]" mode="EnumEntryInitialization">
		<xsl:with-param name="EnumName" select="$EnumName"/>
  </xsl:apply-templates>
</xsl:template>

<!-- =========================================================================== -->
<!-- EnumEntryInitialization                                                     -->
<!-- =========================================================================== -->

<xsl:template match="*" mode="EnumEntryInitialization">
  <xsl:param name="EnumName"/>
  <xsl:param name="NodeType" />
  <xsl:text>static_cast&lt;GenApi::CEnumerationTRef</xsl:text>&lt;<xsl:value-of select="$EnumName"/>Enums&gt; *&gt; (&amp;<xsl:value-of select="$EnumName"/> )<xsl:text>-&gt;SetEnumReference( </xsl:text><xsl:value-of select="$EnumName"/>_<xsl:value-of select="./Symbolic"/>, "<xsl:value-of select="./Symbolic"/>" );<xsl:text>&#x9;&#x9;</xsl:text>
</xsl:template>

</xsl:stylesheet>
