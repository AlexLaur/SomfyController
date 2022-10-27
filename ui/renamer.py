import re
import os


SCRIPT_PATH = os.path.dirname(__file__)
BUILD_PATH = os.path.join(os.path.dirname(SCRIPT_PATH), "data")  # ../data


class Renamer(object):

    HASHED_REGEX = r"([a-fA-F\d]{8})."

    @classmethod
    def rename_build(cls):
        all_files = cls._find_files()

        cls._rename_hashed_files(all_files)
        cls._fix_html_file(all_files)

    @classmethod
    def _rename_hashed_files(cls, all_files):
        for path in all_files:
            if not re.findall(cls.HASHED_REGEX, path):
                continue
            new_path = re.sub(cls.HASHED_REGEX, "", path)

            os.rename(path, new_path)
            print("Rename {} to {}".format(path, new_path))

    @classmethod
    def _fix_html_file(cls, all_files):
        for path in all_files:
            if not path.endswith(".html"):
                continue

            # Read file
            content = []
            with open(path, "r") as f:
                content = f.readlines()

            # Fix it
            new_content = []
            for i in content:
                new_content.append(re.sub(cls.HASHED_REGEX, "", i))

            # rewrite it
            with open(path, "w") as f:
                f.writelines(new_content)

            print("HTML fixed at {}".format(path))

    @staticmethod
    def _find_files():
        result = []
        for root, _, files in os.walk(BUILD_PATH):
            for name in files:
                result.append(os.path.join(root, name))
        return result


if __name__ == "__main__":
    Renamer.rename_build()
