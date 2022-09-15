using System;
using System.Collections.Generic;
using System.Xml;

namespace Generator
{
    class Program
    {
        private const string Defines =
@"
#ifndef CRATER_VK_EXPORTED_FUNCTION
#    define CRATER_VK_EXPORTED_FUNCTION(name)
#endif

#ifndef CRATER_VK_EXT_INSTANCE_FUNCTION
#    define CRATER_VK_EXT_INSTANCE_FUNCTION(name)
#endif

#ifndef CRATER_VK_EXT_DEVICE_FUNCTION
#    define CRATER_VK_EXT_DEVICE_FUNCTION(name)
#endif

";
        private const string Safix =
@"
#ifdef CRATER_VK_EXPORTED_FUNCTION
#undef CRATER_VK_EXPORTED_FUNCTION
#endif

#ifdef CRATER_VK_EXT_INSTANCE_FUNCTION
#undef CRATER_VK_EXT_INSTANCE_FUNCTION
#endif

#ifdef CRATER_VK_EXT_DEVICE_FUNCTION
#undef CRATER_VK_EXT_DEVICE_FUNCTION
#endif
";

        private static void printLevel(int level)
        {
            while(0<level--) {
                Console.Write(' ');
            }
        }

        private static void printSpace()
        {
            Console.Write(' ');
        }

        private static void printLine()
        {
            Console.Write('\n');
        }

        private static void print(Char c)
        {
            Console.Write(c);
        }

        private static void printLine(Char c)
        {
            Console.Write(c);
            printLine();
        }

        private static void print(String s)
        {
            Console.Write(s);
        }

        private static void printLine(String s)
        {
            Console.Write(s);
            printLine();
        }

        private struct Platform
        {
            public string name_;
            public string protect_;
        }

        private struct Function
        {
            public const int Type_Global = 0;
            public const int Type_Instance = 1;
            public const int Type_Device = 2;

            public string name_;
            public string platform_;
            public string protect_;
            public int type_;

            public Function(string name, string platform, string protect, int type)
            {
                name_ = name;
                platform_ = platform;
                protect_ = protect;
                type_ = type;
            }

            public override int GetHashCode()
            {
                return name_.GetHashCode();
            }

            public override bool Equals(object obj)
            {
                if(obj is Function) {
                    Function rhs = (Function)obj;
                    return name_ == rhs.name_;
                }
                return false;
            }
        };

        private static bool gatherGlobalName(Dictionary<string, Function> functions, XmlNode root)
        {
            for(XmlNode node = root.FirstChild; null != node; node=node.NextSibling) {
                if("name" == node.Name) {
                    string name = node.InnerText.Replace(" ", "");
                    if(functions.ContainsKey(name)) {
                        functions[name] = new Function(name, string.Empty, string.Empty, Function.Type_Global);
                    } else {
                        functions.Add(name, new Function(name, string.Empty, string.Empty, Function.Type_Global));
                    }
                    return true;
                }
            }
            return false;
        }

        private static bool gatherVulkanCommand(Dictionary<string, Function> functions, XmlNode root)
        {
            for(XmlNode node = root.FirstChild; null != node;) {
                if("proto" == node.Name) {
                    if(!gatherGlobalName(functions, node)) {
                        return false;
                    }
                    return true;
                } else {
                    node = node.NextSibling;
                }
            }
            return false;
        }

        private static void gatherVulkanCommands(Dictionary<string, Function> functions, XmlNode root)
        {
            for(XmlNode node = root.FirstChild; null != node; node = node.NextSibling) {
                if("command" == node.Name) {
                    gatherVulkanCommand(functions, node);
                }
            }
        }

        private static void gatherPlatforms(Dictionary<string, Platform> platforms, XmlNode root)
        {
            for(XmlNode node = root.FirstChild; null != node; node = node.NextSibling) {
                if("platforms" != node.Name) {
                    continue;
                }
                    foreach(XmlNode child in node.ChildNodes)
                {
                    string name = string.Empty;
                    string protect = string.Empty;
                    foreach(XmlAttribute attr in child.Attributes)
                    {
                        switch (attr.Name)
                        {
                        case "name":
                            name = attr.Value;
                            break;
                        case "protect":
                            protect = attr.Value;
                            break;
                        }
                    }
                    if(string.IsNullOrEmpty(name) || string.IsNullOrEmpty(protect))
                    {
                        continue;
                    }
                    platforms.Add(name, new Platform { name_=name, protect_=protect});
                }
            }
        }

        private static void gatherGlobals(Dictionary<string, Function> functions, XmlNode root)
        {
            for(XmlNode node = root.FirstChild; null != node; node = node.NextSibling) {
                if("commands" == node.Name) {
                    gatherVulkanCommands(functions, node);
                }
            }
        }

        private static void gatherExtension(Dictionary<string, Function> functions, XmlNode root, int type, string platform, string protect)
        {
            for(XmlNode node = root.FirstChild; null != node; node = node.NextSibling) {
                if("command" != node.Name) {
                    continue;
                }
                foreach(XmlAttribute attr in node.Attributes) {
                    switch(attr.Name) {
                    case "name":
                        string name = attr.Value.Replace(" ", "");
                        if(functions.ContainsKey(name)) {
                            functions[name] = new Function(attr.Value, platform, protect, type);
                        } else {
                            functions.Add(name, new Function(attr.Value, platform, protect, type));
                        }
                        break;
                    }
                }
            }
        }

        private static void gatherExtensions(Dictionary<string, Function> functions, XmlNode root)
        {
            for(XmlNode node = root.FirstChild; null != node; node = node.NextSibling) {
                if("extensions" != node.Name) {
                    continue;
                }
                for(XmlNode child = node.FirstChild; null != child; child = child.NextSibling) {
                    if("extension" != child.Name) {
                        continue;
                    }
                    string type = string.Empty;
                    string platform = string.Empty;
                    string protect = string.Empty;
                    foreach(XmlAttribute attr in child.Attributes) {
                        switch(attr.Name) {
                        case "type":
                            type = attr.Value;
                            break;
                        case "platform":
                            platform = attr.Value;
                            break;
                        case "protect":
                            protect = attr.Value;
                            break;
                        }
                    }

                    int itype = Function.Type_Global;
                    switch(type) {
                    case "instance":
                        itype = Function.Type_Instance;
                        break;
                    case "device":
                        itype = Function.Type_Device;
                        break;
                    }
                    for(XmlNode childchild = child.FirstChild; null != childchild; childchild = childchild.NextSibling) {
                        if("require" != childchild.Name) {
                            continue;
                        }
                        gatherExtension(functions, childchild, itype, platform, protect);
                    }
                }//for(XmlNode child
            }//for(XmlNode node
        }

        static void Main(string[] args)
        {
            if(args.Length<=0) {
                return;
            }

            XmlDocument xmlDocument = new XmlDocument();
            try {
                xmlDocument.Load(args[0]);

                Dictionary<string, Platform> platforms = new Dictionary<string, Platform>(32);
                Dictionary<string, Function> functions = new Dictionary<string, Function>(128);
                print(Defines);
                gatherPlatforms(platforms, xmlDocument.DocumentElement);
                gatherGlobals(functions, xmlDocument.DocumentElement);
                gatherExtensions(functions, xmlDocument.DocumentElement);
                foreach(KeyValuePair<string, Function> keyvalue in functions) {
                    bool protect = false;
                    if(!string.IsNullOrEmpty(keyvalue.Value.protect_)) {
                        print("#ifdef ");
                        print(keyvalue.Value.protect_);
                        printLine();
                        protect = true;
                    }else if (!string.IsNullOrEmpty(keyvalue.Value.platform_) && platforms.ContainsKey(keyvalue.Value.platform_))
                    {
                        Platform platform = platforms[keyvalue.Value.platform_];
                        print("#ifdef ");
                        print(platform.protect_);
                        printLine();
                        protect = true;
                    }
                    switch(keyvalue.Value.type_) {
                    case Function.Type_Instance:
                        print("CRATER_VK_EXT_INSTANCE_FUNCTION(");
                        break;
                    case Function.Type_Device:
                        print("CRATER_VK_EXT_DEVICE_FUNCTION(");
                        break;
                    default:
                        print("CRATER_VK_EXPORTED_FUNCTION(");
                        break;
                    }
                    print(keyvalue.Value.name_);
                    printLine(")");
                    if(protect) {
                        printLine("#endif");
                    }
                }
                print(Safix);
            } catch(Exception e) {
                Console.WriteLine(e.Message);
                return;
            }
        }
    }
}
