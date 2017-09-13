//------------------------------------------------------------------------------
// <auto-generated>
//     This code was generated by a tool.
//
//     Changes to this file may cause incorrect behavior and will be lost if
//     the code is regenerated.
// </auto-generated>
//------------------------------------------------------------------------------

// Generated from: friends.proto
namespace NPx
{
  [global::System.Serializable, global::ProtoBuf.ProtoContract(Name=@"FriendsSetSteamIDMessage")]
  public partial class FriendsSetSteamIDMessage : global::ProtoBuf.IExtensible
  {
    public FriendsSetSteamIDMessage() {}
    
    private ulong _steamID;
    [global::ProtoBuf.ProtoMember(1, IsRequired = true, Name=@"steamID", DataFormat = global::ProtoBuf.DataFormat.FixedSize)]
    public ulong steamID
    {
      get { return _steamID; }
      set { _steamID = value; }
    }
    private global::ProtoBuf.IExtension extensionObject;
    global::ProtoBuf.IExtension global::ProtoBuf.IExtensible.GetExtensionObject(bool createIfMissing)
      { return global::ProtoBuf.Extensible.GetExtensionObject(ref extensionObject, createIfMissing); }
  }
  
  [global::System.Serializable, global::ProtoBuf.ProtoContract(Name=@"FriendsGetProfileDataMessage")]
  public partial class FriendsGetProfileDataMessage : global::ProtoBuf.IExtensible
  {
    public FriendsGetProfileDataMessage() {}
    
    private readonly global::System.Collections.Generic.List<ulong> _npids = new global::System.Collections.Generic.List<ulong>();
    [global::ProtoBuf.ProtoMember(1, Name=@"npids", DataFormat = global::ProtoBuf.DataFormat.FixedSize)]
    public global::System.Collections.Generic.List<ulong> npids
    {
      get { return _npids; }
    }
  
    private global::ProtoBuf.IExtension extensionObject;
    global::ProtoBuf.IExtension global::ProtoBuf.IExtensible.GetExtensionObject(bool createIfMissing)
      { return global::ProtoBuf.Extensible.GetExtensionObject(ref extensionObject, createIfMissing); }
  }
  
  [global::System.Serializable, global::ProtoBuf.ProtoContract(Name=@"ProfileDataResult")]
  public partial class ProfileDataResult : global::ProtoBuf.IExtensible
  {
    public ProfileDataResult() {}
    

    private ulong _npid = default(ulong);
    [global::ProtoBuf.ProtoMember(1, IsRequired = false, Name=@"npid", DataFormat = global::ProtoBuf.DataFormat.FixedSize)]
    [global::System.ComponentModel.DefaultValue(default(ulong))]
    public ulong npid
    {
      get { return _npid; }
      set { _npid = value; }
    }

    private byte[] _profile = null;
    [global::ProtoBuf.ProtoMember(2, IsRequired = false, Name=@"profile", DataFormat = global::ProtoBuf.DataFormat.Default)]
    [global::System.ComponentModel.DefaultValue(null)]
    public byte[] profile
    {
      get { return _profile; }
      set { _profile = value; }
    }
    private global::ProtoBuf.IExtension extensionObject;
    global::ProtoBuf.IExtension global::ProtoBuf.IExtensible.GetExtensionObject(bool createIfMissing)
      { return global::ProtoBuf.Extensible.GetExtensionObject(ref extensionObject, createIfMissing); }
  }
  
  [global::System.Serializable, global::ProtoBuf.ProtoContract(Name=@"FriendsGetProfileDataResultMessage")]
  public partial class FriendsGetProfileDataResultMessage : global::ProtoBuf.IExtensible
  {
    public FriendsGetProfileDataResultMessage() {}
    
    private readonly global::System.Collections.Generic.List<ProfileDataResult> _results = new global::System.Collections.Generic.List<ProfileDataResult>();
    [global::ProtoBuf.ProtoMember(1, Name=@"results", DataFormat = global::ProtoBuf.DataFormat.Default)]
    public global::System.Collections.Generic.List<ProfileDataResult> results
    {
      get { return _results; }
    }
  
    private global::ProtoBuf.IExtension extensionObject;
    global::ProtoBuf.IExtension global::ProtoBuf.IExtensible.GetExtensionObject(bool createIfMissing)
      { return global::ProtoBuf.Extensible.GetExtensionObject(ref extensionObject, createIfMissing); }
  }
  
  [global::System.Serializable, global::ProtoBuf.ProtoContract(Name=@"FriendDetails")]
  public partial class FriendDetails : global::ProtoBuf.IExtensible
  {
    public FriendDetails() {}
    

    private ulong _npid = default(ulong);
    [global::ProtoBuf.ProtoMember(1, IsRequired = false, Name=@"npid", DataFormat = global::ProtoBuf.DataFormat.FixedSize)]
    [global::System.ComponentModel.DefaultValue(default(ulong))]
    public ulong npid
    {
      get { return _npid; }
      set { _npid = value; }
    }

    private string _name = "";
    [global::ProtoBuf.ProtoMember(2, IsRequired = false, Name=@"name", DataFormat = global::ProtoBuf.DataFormat.Default)]
    [global::System.ComponentModel.DefaultValue("")]
    public string name
    {
      get { return _name; }
      set { _name = value; }
    }
    private global::ProtoBuf.IExtension extensionObject;
    global::ProtoBuf.IExtension global::ProtoBuf.IExtensible.GetExtensionObject(bool createIfMissing)
      { return global::ProtoBuf.Extensible.GetExtensionObject(ref extensionObject, createIfMissing); }
  }
  
  [global::System.Serializable, global::ProtoBuf.ProtoContract(Name=@"FriendsRosterMessage")]
  public partial class FriendsRosterMessage : global::ProtoBuf.IExtensible
  {
    public FriendsRosterMessage() {}
    
    private readonly global::System.Collections.Generic.List<FriendDetails> _friends = new global::System.Collections.Generic.List<FriendDetails>();
    [global::ProtoBuf.ProtoMember(1, Name=@"friends", DataFormat = global::ProtoBuf.DataFormat.Default)]
    public global::System.Collections.Generic.List<FriendDetails> friends
    {
      get { return _friends; }
    }
  
    private global::ProtoBuf.IExtension extensionObject;
    global::ProtoBuf.IExtension global::ProtoBuf.IExtensible.GetExtensionObject(bool createIfMissing)
      { return global::ProtoBuf.Extensible.GetExtensionObject(ref extensionObject, createIfMissing); }
  }
  
  [global::System.Serializable, global::ProtoBuf.ProtoContract(Name=@"FriendsPresence")]
  public partial class FriendsPresence : global::ProtoBuf.IExtensible
  {
    public FriendsPresence() {}
    
    private string _key;
    [global::ProtoBuf.ProtoMember(1, IsRequired = true, Name=@"key", DataFormat = global::ProtoBuf.DataFormat.Default)]
    public string key
    {
      get { return _key; }
      set { _key = value; }
    }
    private string _value;
    [global::ProtoBuf.ProtoMember(2, IsRequired = true, Name=@"value", DataFormat = global::ProtoBuf.DataFormat.Default)]
    public string value
    {
      get { return _value; }
      set { _value = value; }
    }
    private global::ProtoBuf.IExtension extensionObject;
    global::ProtoBuf.IExtension global::ProtoBuf.IExtensible.GetExtensionObject(bool createIfMissing)
      { return global::ProtoBuf.Extensible.GetExtensionObject(ref extensionObject, createIfMissing); }
  }
  
  [global::System.Serializable, global::ProtoBuf.ProtoContract(Name=@"FriendsPresenceMessage")]
  public partial class FriendsPresenceMessage : global::ProtoBuf.IExtensible
  {
    public FriendsPresenceMessage() {}
    
    private ulong _friend;
    [global::ProtoBuf.ProtoMember(1, IsRequired = true, Name=@"friend", DataFormat = global::ProtoBuf.DataFormat.FixedSize)]
    public ulong friend
    {
      get { return _friend; }
      set { _friend = value; }
    }
    private int _presenceState;
    [global::ProtoBuf.ProtoMember(2, IsRequired = true, Name=@"presenceState", DataFormat = global::ProtoBuf.DataFormat.TwosComplement)]
    public int presenceState
    {
      get { return _presenceState; }
      set { _presenceState = value; }
    }

    private ulong _currentServer = default(ulong);
    [global::ProtoBuf.ProtoMember(3, IsRequired = false, Name=@"currentServer", DataFormat = global::ProtoBuf.DataFormat.FixedSize)]
    [global::System.ComponentModel.DefaultValue(default(ulong))]
    public ulong currentServer
    {
      get { return _currentServer; }
      set { _currentServer = value; }
    }
    private readonly global::System.Collections.Generic.List<FriendsPresence> _presence = new global::System.Collections.Generic.List<FriendsPresence>();
    [global::ProtoBuf.ProtoMember(4, Name=@"presence", DataFormat = global::ProtoBuf.DataFormat.Default)]
    public global::System.Collections.Generic.List<FriendsPresence> presence
    {
      get { return _presence; }
    }
  
    private global::ProtoBuf.IExtension extensionObject;
    global::ProtoBuf.IExtension global::ProtoBuf.IExtensible.GetExtensionObject(bool createIfMissing)
      { return global::ProtoBuf.Extensible.GetExtensionObject(ref extensionObject, createIfMissing); }
  }
  
  [global::System.Serializable, global::ProtoBuf.ProtoContract(Name=@"FriendsSetPresenceMessage")]
  public partial class FriendsSetPresenceMessage : global::ProtoBuf.IExtensible
  {
    public FriendsSetPresenceMessage() {}
    
    private readonly global::System.Collections.Generic.List<FriendsPresence> _presence = new global::System.Collections.Generic.List<FriendsPresence>();
    [global::ProtoBuf.ProtoMember(1, Name=@"presence", DataFormat = global::ProtoBuf.DataFormat.Default)]
    public global::System.Collections.Generic.List<FriendsPresence> presence
    {
      get { return _presence; }
    }
  
    private global::ProtoBuf.IExtension extensionObject;
    global::ProtoBuf.IExtension global::ProtoBuf.IExtensible.GetExtensionObject(bool createIfMissing)
      { return global::ProtoBuf.Extensible.GetExtensionObject(ref extensionObject, createIfMissing); }
  }
  
  [global::System.Serializable, global::ProtoBuf.ProtoContract(Name=@"FriendsGetUserAvatarMessage")]
  public partial class FriendsGetUserAvatarMessage : global::ProtoBuf.IExtensible
  {
    public FriendsGetUserAvatarMessage() {}
    
    private int _guid;
    [global::ProtoBuf.ProtoMember(2, IsRequired = true, Name=@"guid", DataFormat = global::ProtoBuf.DataFormat.TwosComplement)]
    public int guid
    {
      get { return _guid; }
      set { _guid = value; }
    }
    private global::ProtoBuf.IExtension extensionObject;
    global::ProtoBuf.IExtension global::ProtoBuf.IExtensible.GetExtensionObject(bool createIfMissing)
      { return global::ProtoBuf.Extensible.GetExtensionObject(ref extensionObject, createIfMissing); }
  }
  
  [global::System.Serializable, global::ProtoBuf.ProtoContract(Name=@"FriendsGetUserAvatarResultMessage")]
  public partial class FriendsGetUserAvatarResultMessage : global::ProtoBuf.IExtensible
  {
    public FriendsGetUserAvatarResultMessage() {}
    
    private int _result;
    [global::ProtoBuf.ProtoMember(1, IsRequired = true, Name=@"result", DataFormat = global::ProtoBuf.DataFormat.TwosComplement)]
    public int result
    {
      get { return _result; }
      set { _result = value; }
    }
    private int _guid;
    [global::ProtoBuf.ProtoMember(2, IsRequired = true, Name=@"guid", DataFormat = global::ProtoBuf.DataFormat.TwosComplement)]
    public int guid
    {
      get { return _guid; }
      set { _guid = value; }
    }
    private byte[] _fileData;
    [global::ProtoBuf.ProtoMember(3, IsRequired = true, Name=@"fileData", DataFormat = global::ProtoBuf.DataFormat.Default)]
    public byte[] fileData
    {
      get { return _fileData; }
      set { _fileData = value; }
    }
    private global::ProtoBuf.IExtension extensionObject;
    global::ProtoBuf.IExtension global::ProtoBuf.IExtensible.GetExtensionObject(bool createIfMissing)
      { return global::ProtoBuf.Extensible.GetExtensionObject(ref extensionObject, createIfMissing); }
  }
  
}